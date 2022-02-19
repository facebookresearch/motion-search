#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <emmintrin.h>

// The following guarantee there is enough room for diamond search to search in extended area
#define HORIZONTAL_PADDING 32
#define VERTICAL_PADDING 24
#define HOR_PADDING_UV (HORIZONTAL_PADDING/2)
#define VER_PADDING_UV (VERTICAL_PADDING/2)

// USE_MMX_SSE should only be enabled on a Pentium 4 or higher Intel CPU
#define USE_MMX_SSE

// Currently, MB_WIDTH can take values 4, 8 or 16
#define MB_WIDTH 16
//#define MB_WIDTH 8
//#define MB_WIDTH 4

// SEARCH_MV switches between PMVFAST and full_search
#define SEARCH_MV PMVFAST
//#define SEARCH_MV full_search

// There are 2 strategies for "median" predictor in PMVFAST, mean or median
#define USE_MEAN

// Use total energy or DC-removed (i.e. AC_ENERGY) for MSE and bi-directional MSE
#define AC_ENERGY

// The value to use for the frame border; smaller value forces diamond search
//#define BORDER_SADS 2*MB_WIDTH*MB_WIDTH
#define BORDER_SADS 0

//We are weighting I-frames by 10% more bits, since QP needs to be the lowest among I/P/B
#define I_FRAME_BIT_WEIGHT ((int) (1.10*256+0.5))
//We are weighting P-frames by 5% more bits, since QP needs to be lower than B (but higher than I)
#define P_FRAME_BIT_WEIGHT ((int) (1.05*256+0.5))
//We are weighting B-frames by 0% more bits, since QP needs to be highest among I/P/B
#define B_FRAME_BIT_WEIGHT ((int) (1.00*256+0.5))

//We are weighting 16x16 MSE by -12.5%, since 8x8 will require more bits
#define NORMALIZE(x) ((x*7+4)>>3)

typedef struct {
	// y component first, since it corresponds to row
	// x component second - column
	short y;
	short x;
} MV;

#ifdef __cplusplus
}
#endif
