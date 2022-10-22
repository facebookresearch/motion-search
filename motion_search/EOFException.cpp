/*
 Copyright (c) Meta Platforms, Inc. and affiliates.

 This source code is licensed under the BSD3 license found in the
 LICENSE file in the root directory of this source tree.
 */

#include "EOFException.h"

EOFException::EOFException(void) : runtime_error("EOF Reached") {}

EOFException::~EOFException(void) {}
