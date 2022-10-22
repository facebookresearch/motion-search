
/*
 Copyright (c) Meta Platforms, Inc. and affiliates.

 This source code is licensed under the BSD3 license found in the
 LICENSE file in the root directory of this source tree.
 */

#include "YUVFrame.h"

#include "frame.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>

YUVFrame::YUVFrame(IVideoSequenceReader *rdr)
    : m_dim(rdr->dim()), m_stride(rdr->dim().width + 2 * HORIZONTAL_PADDING),
      m_padded_height(rdr->dim().height + 2 * VERTICAL_PADDING), m_pReader(rdr),
      m_pos(-1) {
  size_t frame_size = m_stride * m_padded_height * 3 * sizeof(uint8_t) / 2;

  m_pFrame = memory::AlignedAlloc<uint8_t>(frame_size);
  if (m_pFrame == NULL) {
    printf("Not enough memory (%zu bytes) for YUVFrame\n", frame_size);
    exit(-1);
  }

  int luma_offset = VERTICAL_PADDING * m_stride + HORIZONTAL_PADDING;
  int cr_offset = m_stride * m_padded_height + VER_PADDING_UV * m_stride / 2 +
                  HOR_PADDING_UV;
  int cb_offset = cr_offset + (m_stride / 2) * (m_padded_height / 2);

  m_pY = m_pFrame.get() + luma_offset;
  m_pU = m_pFrame.get() + cr_offset;
  m_pV = m_pFrame.get() + cb_offset;
}

void YUVFrame::swapFrame(YUVFrame *other) {
  std::swap(this->m_pFrame, other->m_pFrame);
  std::swap(this->m_pY, other->m_pY);
  std::swap(this->m_pU, other->m_pU);
  std::swap(this->m_pV, other->m_pV);
  std::swap(this->m_pos, other->m_pos);
}

void YUVFrame::readNextFrame(void) {
  m_pos = m_pReader->count();
  m_pReader->read(y(), u(), v());
}

void YUVFrame::boundaryExtend(void) {
  extend_frame(y(), m_stride, m_dim, HORIZONTAL_PADDING, VERTICAL_PADDING);
  extend_frame(u(), m_stride >> 1, m_dim / 2, HOR_PADDING_UV, VER_PADDING_UV);
  extend_frame(v(), m_stride >> 1, m_dim / 2, HOR_PADDING_UV, VER_PADDING_UV);
}
