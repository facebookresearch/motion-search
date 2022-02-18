#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"

int fullSAD(unsigned char *current, unsigned char *reference, int stride, int block_width, int block_height, int min_SAD);
int partialSAD(unsigned char *current, unsigned char *reference, int stride, int block_width, int block_height, int min_SAD);
int variance(unsigned char *current, int stride, int block_width, int block_height);
int calc_mse(unsigned char *current, unsigned char *reference, int stride, int block_width, int block_height);
int bidir_mse(unsigned char *current, unsigned char *reference1, unsigned char *reference2, int stride, int block_width, int block_height, MV *td);
#if defined(USE_MMX_SSE)

int fastSAD16(unsigned char *current, unsigned char *reference, int stride, int block_width, int block_height, int min_SAD);
int fastSAD8(unsigned char *current, unsigned char *reference, int stride, int block_width, int block_height, int min_SAD);
int fastSAD4(unsigned char *current, unsigned char *reference, int stride, int block_width, int block_height, int min_SAD);
int fast_variance16(unsigned char *current, int stride, int block_width, int block_height);
int fast_variance8(unsigned char *current, int stride, int block_width, int block_height);
int fast_variance4(unsigned char *current, int stride, int block_width, int block_height);
int fast_calc_mse16(unsigned char *current, unsigned char *reference, int stride, int block_width, int block_height);
int fast_calc_mse8(unsigned char *current, unsigned char *reference, int stride, int block_width, int block_height);
int fast_calc_mse4(unsigned char *current, unsigned char *reference, int stride, int block_width, int block_height);
int fast_bidir_mse16(unsigned char *current, unsigned char *reference1, unsigned char *reference2, int stride, int block_width, int block_height, MV *td);
int fast_bidir_mse8(unsigned char *current, unsigned char *reference1, unsigned char *reference2, int stride, int block_width, int block_height, MV *td);
int fast_bidir_mse4(unsigned char *current, unsigned char *reference1, unsigned char *reference2, int stride, int block_width, int block_height, MV *td);

#else

#define fastSAD16 partialSAD
#define fastSAD8 partialSAD
#define fastSAD4 partialSAD
#define fast_variance16 variance
#define fast_variance8 variance
#define fast_variance4 variance
#define fast_calc_mse16 calc_mse
#define fast_calc_mse8 calc_mse
#define fast_calc_mse4 calc_mse
#define fast_bidir_mse16 bidir_mse
#define fast_bidir_mse8 bidir_mse
#define fast_bidir_mse4 bidir_mse

#endif // USE_MMX_SSE

#ifdef __cplusplus
}
#endif
