/*
 Copyright (c) Meta Platforms, Inc. and affiliates.

 This source code is licensed under the BSD3 license found in the
 LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "common.h"
#include <stdexcept>

using std::runtime_error;

class EOFException : public runtime_error {
public:
  EOFException(void);
  ~EOFException(void);
};
