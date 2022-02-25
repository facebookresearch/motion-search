
#pragma once

#include <motion_search/inc/xplatform.h>

#if defined(_WINDOWS)
// 4514 unreferenced inline function has been removed
// 4710 function not inlined
// 5045 Compiler will insert Spectre mitigation for memory load
#pragma warning(disable: 4514 4710 5045)
#endif // !defined(_WINDOWS)

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// The following guarantee there is enough room for diamond search to search in extended area
#define HORIZONTAL_PADDING 32
#define VERTICAL_PADDING 24
#define HOR_PADDING_UV (HORIZONTAL_PADDING/2)
#define VER_PADDING_UV (VERTICAL_PADDING/2)

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

typedef struct MV {
    // y component first, since it corresponds to row
    // x component second - column
    short y;
    short x;
} MV;

typedef struct DIM {
    int32_t width;
    int32_t height;
} DIM;

#ifdef __cplusplus
} // extern "C" {

inline
DIM operator / (const DIM &left, const int32_t right) {
    DIM dim = {
        left.width / right, dim.height / right};
    return dim;
}

#endif // __cplusplus
