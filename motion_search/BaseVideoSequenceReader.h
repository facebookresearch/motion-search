/*
 Copyright (c) Meta Platforms, Inc. and affiliates.

 This source code is licensed under the BSD3 license found in the
 LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "IVideoSequenceReader.h"
#include "common.h"

class BaseVideoSequenceReader : public IVideoSequenceReader {
public:
  BaseVideoSequenceReader(void) = default;
  virtual ~BaseVideoSequenceReader(void) = default;

  void read(uint8_t *pY, uint8_t *pU, uint8_t *pV) override;

  int count() override { return m_count; }

protected:
  virtual void readPicture(uint8_t *pY, uint8_t *pU, uint8_t *pV) = 0;

private:
  int m_count = 0;
};
