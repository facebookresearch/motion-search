/*
 Copyright (c) Meta Platforms, Inc. and affiliates.

 This source code is licensed under the BSD3 license found in the
 LICENSE file in the root directory of this source tree.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include "string.h"

void extend_frame(uint8_t *frame_ptr, const ptrdiff_t stride, const DIM dim,
                  int pad_size_x, int pad_size_y);

#ifdef __cplusplus
}
#endif
