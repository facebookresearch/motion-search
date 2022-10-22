
/*
 Copyright (c) Meta Platforms, Inc. and affiliates.

 This source code is licensed under the BSD3 license found in the
 LICENSE file in the root directory of this source tree.
 */

#include "moments.h"

#include <stdlib.h>

#if 0
int fullSAD(unsigned char *current, unsigned char *reference, int stride,
    int block_width, int block_height, int min_SAD)
{
    UNUSED(min_SAD);

    int temp_SAD;
    int diff;
    int i, j;

    temp_SAD = 0;
    for(i=block_height;i>0;i--)
    {
        for(j=0;j<block_width;j++)
        {
            diff = abs(current[j]-reference[j]);
            temp_SAD += diff;
        }
        current += stride;
        reference += stride;
    }

    return temp_SAD;
}
#endif

// WxH SAD using early termination
static int partialSAD(FAST_SAD_FORMAL_ARGS) {
  int temp_SAD;
  int diff;
  int i, j;

  temp_SAD = min_SAD;
  for (i = block_height; i > 0 && temp_SAD > 0; i--) {
    for (j = 0; j < block_width; j++) {
      diff = abs(current[j] - reference[j]);
      temp_SAD -= diff;
    }
    current += stride;
    reference += stride;
  }

  return min_SAD - temp_SAD;
}

int fastSAD16_c(FAST_SAD_FORMAL_ARGS) {
  return partialSAD(FAST_SAD_ACTUAL_ARGS);
}

int fastSAD8_c(FAST_SAD_FORMAL_ARGS) {
  return partialSAD(FAST_SAD_ACTUAL_ARGS);
}

int fastSAD4_c(FAST_SAD_FORMAL_ARGS) {
  return partialSAD(FAST_SAD_ACTUAL_ARGS);
}

// Return block variance, multiplied by block_width*block_height
static int variance(FAST_VARIANCE_FORMAL_ARGS) {
  int i, j;
  int temp;
  int sum;
  int sum2;

  sum = sum2 = 0;
  for (i = block_height; i > 0; i--) {
    for (j = 0; j < block_width; j++) {
      temp = current[j];
      sum += temp;
      sum2 += temp * temp;
    }
    current += stride;
  }

  temp = block_height * block_width;
  return sum2 - (sum * sum + (temp >> 1)) / temp;
}

int fast_variance16_c(FAST_VARIANCE_FORMAL_ARGS) {
  return variance(FAST_VARIANCE_ACTUAL_ARGS);
}

int fast_variance8_c(FAST_VARIANCE_FORMAL_ARGS) {
  return variance(FAST_VARIANCE_ACTUAL_ARGS);
}

int fast_variance4_c(FAST_VARIANCE_FORMAL_ARGS) {
  return variance(FAST_VARIANCE_ACTUAL_ARGS);
}

// Sum of square differences
static int calc_mse(FAST_MSE_FORMAL_ARGS) {
  int i, j;
  int temp;
#ifdef AC_ENERGY
  int sum;
#endif // AC_ENERGY
  int sum2;

#ifdef AC_ENERGY
  sum = 0;
#endif // AC_ENERGY
  sum2 = 0;
  for (i = block_height; i > 0; i--) {
    for (j = 0; j < block_width; j++) {
      temp = current[j] - reference[j];
#ifdef AC_ENERGY
      sum += temp;
#endif // AC_ENERGY
      sum2 += temp * temp;
    }
    current += stride;
    reference += stride;
  }

#ifdef AC_ENERGY
  temp = block_height * block_width;
  sum2 -= (sum * sum + (temp >> 1)) / temp;
#endif // AC_ENERGY
  return sum2;
}

int fast_calc_mse16_c(FAST_MSE_FORMAL_ARGS) {
  return calc_mse(FAST_MSE_ACTUAL_ARGS);
}

int fast_calc_mse8_c(FAST_MSE_FORMAL_ARGS) {
  return calc_mse(FAST_MSE_ACTUAL_ARGS);
}

int fast_calc_mse4_c(FAST_MSE_FORMAL_ARGS) {
  return calc_mse(FAST_MSE_ACTUAL_ARGS);
}

// Sum of square differences after bi-directional interpolation; we assume
// td1+td2 = 32768
static int bidir_mse(FAST_BIDIR_MSE_FORMAL_ARGS) {
  int i, j;
  int temp;
#ifdef AC_ENERGY
  int sum;
#endif // AC_ENERGY
  int sum2;

#ifdef AC_ENERGY
  sum = 0;
#endif // AC_ENERGY
  sum2 = 0;
  for (i = block_height; i > 0; i--) {
    for (j = 0; j < block_width; j++) {
      temp =
          current[j] -
          ((reference1[j] * (td->y) + reference2[j] * (td->x) + 16384) >> 15);
#ifdef AC_ENERGY
      sum += temp;
#endif // AC_ENERGY
      sum2 += temp * temp;
    }
    current += stride;
    reference1 += stride;
    reference2 += stride;
  }

#ifdef AC_ENERGY
  temp = block_height * block_width;
  sum2 -= (sum * sum + (temp >> 1)) / temp;
#endif // AC_ENERGY
  return sum2;
}

int fast_bidir_mse16_c(FAST_BIDIR_MSE_FORMAL_ARGS) {
  return bidir_mse(FAST_BIDIR_MSE_ACTUAL_ARGS);
}

int fast_bidir_mse8_c(FAST_BIDIR_MSE_FORMAL_ARGS) {
  return bidir_mse(FAST_BIDIR_MSE_ACTUAL_ARGS);
}

int fast_bidir_mse4_c(FAST_BIDIR_MSE_FORMAL_ARGS) {
  return bidir_mse(FAST_BIDIR_MSE_ACTUAL_ARGS);
}
