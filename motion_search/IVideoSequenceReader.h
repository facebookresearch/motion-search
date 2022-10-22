/*
 Copyright (c) Meta Platforms, Inc. and affiliates.

 This source code is licensed under the BSD3 license found in the
 LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "common.h"

class IVideoSequenceReader {
public:
  virtual ~IVideoSequenceReader(){};
  virtual void read(uint8_t *pY, uint8_t *pU, uint8_t *pV) = 0;
  virtual bool eof(void) = 0;
  virtual int nframes(void) = 0;
  virtual int count(void) = 0;
  virtual const DIM dim(void) = 0;
  virtual ptrdiff_t stride(void) = 0;
  virtual bool isOpen(void) = 0;
};
