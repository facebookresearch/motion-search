
/*
 Copyright (c) Meta Platforms, Inc. and affiliates.

 This source code is licensed under the BSD3 license found in the
 LICENSE file in the root directory of this source tree.
 */

#include "Y4MSequenceReader.h"

#include "EOFException.h"

namespace {

enum { HEADER_SIZE = 4096 };

namespace Parameters {
const char *const Signature = "YUV4MPEG2 ";
const char *const Width = " W";
const char *const Height = " H";
const char *const Frame = "FRAME\n";
}; // namespace Parameters

} // namespace

bool Y4MSequenceReader::Open(unique_file_t file, const std::string &path) {
  if (!file) {
    return false;
  }

  DIM dim;
  char header[HEADER_SIZE];

  auto res = fseek(file.get(), 0, SEEK_SET);
  if (res) {
    return false;
  }
  auto headerSize = fread(header, 1, HEADER_SIZE - 1, file.get());
  header[headerSize] = 0;

  // read parameters
  auto p = strstr(header, Parameters::Signature);
  if (p != header) {
    return false;
  }
  p = strstr(header, Parameters::Width);
  if (!p) {
    return false;
  }
  dim.width = atoi(p + 2);
  p = strstr(header, Parameters::Height);
  if (!p) {
    return false;
  }
  dim.height = atoi(p + 2);
  //
  // parse additional parameters here: format, frame rate, aspect ratio...
  //

  // find the first frame
  p = strstr(header, Parameters::Frame);
  if (!p) {
    return false;
  }
  res = fseek(file.get(), (long)(p - header), SEEK_SET);
  if (res) {
    return false;
  }

  return YUVSequenceReader::Open(std::move(file), path, dim);
}

void Y4MSequenceReader::readPicture(uint8_t *pY, uint8_t *pU, uint8_t *pV) {
  fseek(file(), (long)strlen(Parameters::Frame), SEEK_CUR);
  YUVSequenceReader::readPicture(pY, pU, pV);
}
