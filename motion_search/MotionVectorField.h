/*
 Copyright (c) Meta Platforms, Inc. and affiliates.

 This source code is licensed under the BSD3 license found in the
 LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "YUVFrame.h"

#include "memory.h"

class MotionVectorField {
public:
  MotionVectorField(const DIM dim, int stride, int padded_height,
                    int blocksize);
  virtual ~MotionVectorField(void) = default;

  int predictSpatial(YUVFrame *pFrm, int *mses, unsigned char *MB_modes);

  int predictTemporal(YUVFrame *pCurFrm, YUVFrame *pRefFrm, int *mses,
                      unsigned char *MB_modes);

  int predictBidirectional(YUVFrame *pCurFrm, YUVFrame *pRefFrm1,
                           YUVFrame *pRefFrm2, MotionVectorField *fwdref,
                           MotionVectorField *bckref, int *mses,
                           unsigned char *MB_modes);

  void reset(void);

  inline int blocksize(void) { return m_blocksize; }

  inline int count_I(void) { return m_count_I; }

  inline int count_P(void) { return m_count_P; }

  inline int count_B(void) { return m_count_B; }

  inline int bits(void) { return m_bits; }

  inline int firstMB(void) { return m_firstMB; }

  inline MV *MVs(void) { return &m_pMVs.get()[m_firstMB]; }

  inline int *SADs(void) { return &m_pSADs.get()[m_firstMB]; }

private:
  memory::aligned_unique_ptr<MV> m_pMVs;
  memory::aligned_unique_ptr<int> m_pSADs;
  size_t m_num_blocks = 0;
  int m_firstMB = 0;

  int m_blocksize;

  int m_count_I = 0;
  int m_count_P = 0;
  int m_count_B = 0;
  int m_bits = 0;

  MotionVectorField(MotionVectorField &) = delete;
  MotionVectorField &operator=(MotionVectorField &) = delete;
};
