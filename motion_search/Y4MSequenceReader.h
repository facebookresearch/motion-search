
/*
 Copyright (c) Meta Platforms, Inc. and affiliates.

 This source code is licensed under the BSD3 license found in the
 LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "YUVSequenceReader.h"

#include <memory>
#include <string>

class Y4MSequenceReader : public YUVSequenceReader {
public:
  Y4MSequenceReader(void) = default;
  ~Y4MSequenceReader(void) = default;

  bool Open(unique_file_t file, const std::string &path);

protected:
  void readPicture(uint8_t *pY, uint8_t *pU, uint8_t *pV) override;

private:
  Y4MSequenceReader(Y4MSequenceReader &) = delete;
  Y4MSequenceReader &operator=(Y4MSequenceReader &) = delete;
};
