/*
 Copyright (c) Meta Platforms, Inc. and affiliates.

 This source code is licensed under the BSD3 license found in the
 LICENSE file in the root directory of this source tree.
 */

#include "BaseVideoSequenceReader.h"

void BaseVideoSequenceReader::read(uint8_t *pY, uint8_t *pU, uint8_t *pV) {
  readPicture(pY, pU, pV);
  m_count++;
}
