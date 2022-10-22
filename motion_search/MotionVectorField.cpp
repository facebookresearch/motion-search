/*
 Copyright (c) Meta Platforms, Inc. and affiliates.

 This source code is licensed under the BSD3 license found in the
 LICENSE file in the root directory of this source tree.
 */

#include "MotionVectorField.h"

#include "motion_search.h"

MotionVectorField::MotionVectorField(const DIM dim, int stride,
                                     int padded_height, int blocksize)
    : m_firstMB(dim.width / MB_WIDTH + 2 + 1), m_blocksize(blocksize),
      m_count_I(0), m_count_P(0), m_count_B(0) {
  int i, j;
  int stride_MB = dim.width / MB_WIDTH + 2;
  int padded_height_MB = (dim.height + MB_WIDTH - 1) / MB_WIDTH + 2;
  m_num_blocks = (size_t)stride_MB * padded_height_MB;

  m_pMVs = memory::AlignedAlloc<MV>(m_num_blocks);
  if (m_pMVs == NULL) {
    printf("Not enough memory (%zu bytes) for motion vectors\n",
           m_num_blocks * sizeof(MV));
    exit(-1);
  }

  m_pSADs = memory::AlignedAlloc<int>(m_num_blocks);
  if (m_pSADs == NULL) {
    printf("Not enough memory (%zu bytes) for SADs\n",
           m_num_blocks * sizeof(int));
    exit(-1);
  }

  for (j = 0; j < stride_MB; j++) {
    m_pSADs.get()[j] = BORDER_SADS;
  }
  for (i = 1; i < padded_height_MB - 1; i++) {
    m_pSADs.get()[i * stride_MB] = m_pSADs.get()[(i + 1) * stride_MB - 1] =
        BORDER_SADS;
  }
  for (j = 0; j < stride_MB; j++) {
    m_pSADs.get()[i * stride_MB + j] = BORDER_SADS;
  }
}

int MotionVectorField::predictSpatial(YUVFrame *pFrm, int *mses,
                                      unsigned char *MB_modes) {
  return spatial_search(pFrm->y(), pFrm->y(), pFrm->stride(), pFrm->dim(),
                        m_blocksize, m_blocksize, &m_pMVs.get()[m_firstMB],
                        &m_pSADs.get()[m_firstMB], mses, MB_modes, &m_count_I,
                        &m_bits);
}

int MotionVectorField::predictTemporal(YUVFrame *pCurFrm, YUVFrame *pRefFrm,
                                       int *mses, unsigned char *MB_modes) {
  return motion_search(pCurFrm->y(), pRefFrm->y(), pCurFrm->stride(),
                       pCurFrm->dim(), m_blocksize, m_blocksize,
                       &m_pMVs.get()[m_firstMB], &m_pSADs.get()[m_firstMB],
                       mses, MB_modes, &m_count_I, &m_count_P, &m_bits);
}

int MotionVectorField::predictBidirectional(
    YUVFrame *pCurFrm, YUVFrame *pRefFrm1, YUVFrame *pRefFrm2,
    MotionVectorField *fwdref, MotionVectorField *bckref, int *mses,
    unsigned char *MB_modes) {
  int pos = pCurFrm->pos() - pRefFrm1->pos();
  int total = pRefFrm2->pos() - pRefFrm1->pos();
  short td1, td2;

  td1 = (short)((pos * 32768 + total / 2) / total);
  td2 = (short)(32768 - td1);
  return bidir_motion_search(
      pCurFrm->y(), pRefFrm1->y(), pRefFrm2->y(), pCurFrm->stride(),
      pCurFrm->dim(), m_blocksize, m_blocksize, this->MVs(), fwdref->MVs(),
      bckref->MVs(), fwdref->SADs(), bckref->SADs(), mses, MB_modes, td1, td2,
      &m_count_I, &m_count_P, &m_count_B, &m_bits);
}

void MotionVectorField::reset(void) {
  memset(m_pMVs.get(), 0, m_num_blocks * sizeof(MV));
}
