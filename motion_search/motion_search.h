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

int spatial_search(unsigned char *current, unsigned char *reference, int stride,
                   const DIM dim, int block_width, int block_height,
                   MV *motion_vectors, int *SADs, int *mses,
                   unsigned char *MB_modes, int *count_I, int *bits);
int motion_search(unsigned char *current, unsigned char *reference, int stride,
                  const DIM dim, int block_width, int block_height,
                  MV *motion_vectors, int *SADs, int *mses,
                  unsigned char *MB_modes, int *count_I, int *count_P,
                  int *bits);
int bidir_motion_search(unsigned char *current, unsigned char *reference1,
                        unsigned char *reference2, int stride, const DIM dim,
                        int block_width, int block_height, MV *P_motion_vectors,
                        MV *motion_vectors1, MV *motion_vectors2, int *SADs1,
                        int *SADs2, int *mses, unsigned char *MB_modes,
                        short td1, short td2, int *count_I, int *count_P,
                        int *count_B, int *bits);

#ifdef __cplusplus
}
#endif
