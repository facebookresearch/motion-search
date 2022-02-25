
#pragma once

#include <motion_search/inc/common.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
    declare optimized functions callers
*/

#define FAST_SAD_FORMAL_ARGS \
    const uint8_t *current, const uint8_t *reference, const ptrdiff_t stride, \
    int block_width, int block_height, int min_SAD
#define FAST_SAD_ACTUAL_ARGS \
    current, reference, stride, block_width, block_height, min_SAD

int fastSAD16(FAST_SAD_FORMAL_ARGS);
int fastSAD8(FAST_SAD_FORMAL_ARGS);
int fastSAD4(FAST_SAD_FORMAL_ARGS);

#define FAST_VARIANCE_FORMAL_ARGS \
    const uint8_t *current, const ptrdiff_t stride, int block_width, int block_height
#define FAST_VARIANCE_ACTUAL_ARGS \
    current, stride, block_width, block_height

int fast_variance16(FAST_VARIANCE_FORMAL_ARGS);
int fast_variance8(FAST_VARIANCE_FORMAL_ARGS);
int fast_variance4(FAST_VARIANCE_FORMAL_ARGS);

#define FAST_MSE_FORMAL_ARGS \
    const uint8_t *current, const uint8_t *reference, const ptrdiff_t stride, \
    int block_width, int block_height
#define FAST_MSE_ACTUAL_ARGS \
    current, reference, stride, block_width, block_height

int fast_calc_mse16(FAST_MSE_FORMAL_ARGS);
int fast_calc_mse8(FAST_MSE_FORMAL_ARGS);
int fast_calc_mse4(FAST_MSE_FORMAL_ARGS);

#define FAST_BIDIR_MSE_FORMAL_ARGS \
    const uint8_t *current, const uint8_t *reference1, const uint8_t *reference2, \
    const ptrdiff_t stride, int block_width, int block_height, MV *td
#define FAST_BIDIR_MSE_ACTUAL_ARGS \
    current, reference1, reference2, stride, block_width, block_height, td

int fast_bidir_mse16(FAST_BIDIR_MSE_FORMAL_ARGS);
int fast_bidir_mse8(FAST_BIDIR_MSE_FORMAL_ARGS);
int fast_bidir_mse4(FAST_BIDIR_MSE_FORMAL_ARGS);

/*
    declare reference functions
*/

int fastSAD16_c(FAST_SAD_FORMAL_ARGS);
int fastSAD8_c(FAST_SAD_FORMAL_ARGS);
int fastSAD4_c(FAST_SAD_FORMAL_ARGS);

int fast_variance16_c(FAST_VARIANCE_FORMAL_ARGS);
int fast_variance8_c(FAST_VARIANCE_FORMAL_ARGS);
int fast_variance4_c(FAST_VARIANCE_FORMAL_ARGS);

int fast_calc_mse16_c(FAST_MSE_FORMAL_ARGS);
int fast_calc_mse8_c(FAST_MSE_FORMAL_ARGS);
int fast_calc_mse4_c(FAST_MSE_FORMAL_ARGS);

int fast_bidir_mse16_c(FAST_BIDIR_MSE_FORMAL_ARGS);
int fast_bidir_mse8_c(FAST_BIDIR_MSE_FORMAL_ARGS);
int fast_bidir_mse4_c(FAST_BIDIR_MSE_FORMAL_ARGS);

/*
    declare optimized functions
*/

#if defined(_X86) || defined(_X64)

int fastSAD16_sse2(FAST_SAD_FORMAL_ARGS);
int fastSAD8_sse2(FAST_SAD_FORMAL_ARGS);
int fastSAD4_sse2(FAST_SAD_FORMAL_ARGS);

int fast_variance16_sse2(FAST_VARIANCE_FORMAL_ARGS);
int fast_variance8_sse2(FAST_VARIANCE_FORMAL_ARGS);
int fast_variance4_sse2(FAST_VARIANCE_FORMAL_ARGS);

int fast_calc_mse16_sse2(FAST_MSE_FORMAL_ARGS);
int fast_calc_mse8_sse2(FAST_MSE_FORMAL_ARGS);
int fast_calc_mse4_sse2(FAST_MSE_FORMAL_ARGS);

int fast_bidir_mse16_sse2(FAST_BIDIR_MSE_FORMAL_ARGS);
int fast_bidir_mse8_sse2(FAST_BIDIR_MSE_FORMAL_ARGS);
int fast_bidir_mse4_sse2(FAST_BIDIR_MSE_FORMAL_ARGS);

#elif defined(_ARM) || defined(_ARM64)

#endif /* defined(_X86) || defined(_X64) */

#ifdef __cplusplus
} // extern "C"
#endif
