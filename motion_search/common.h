#pragma once

//
// define the platform
//

#if defined(_MSC_VER) || defined(_WIN32) || defined(_WIN64)
#if !defined(_WINDOWS)
#define _WINDOWS 1
#endif // !defined(_WINDOWS)
// 4514 unreferenced inline function has been removed
// 4710 function not inlined
// 5045 Compiler will insert Spectre mitigation for memory load
#pragma warning(disable: 4514 4710 5045)
#else // !(defined(_MSC_VER) || defined(_WIN32) || defined(_WIN64))
#if !defined(_LINUX)
#define _LINUX 1
#endif // !defined(_LINUX)
#endif // defined(_MSC_VER) || defined(_WIN32) || defined(_WIN64)

#if defined(_M_IX86) || (defined(__GNUC__) && defined(__i386__))
#if !defined(_X86)
#define _X86 1
#endif // !defined(_X86)
#endif // defined(_M_IX86) || (defined(__GNUC__) && defined(__i386__))

#if defined(_M_X64) || (defined(__GNUC__) && defined(__x86_64__)) || defined(__amd64__)
#if !defined(_X64)
#define _X64 1
#endif // !defined(_X64)
#endif // defined(_M_X64) || (defined(__GNUC__) && defined(__x86_64__)) || defined(__amd64__)

#include <stdint.h>


#if defined(_X86) || defined(_X64)

#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(_LINUX)
#include <x86intrin.h>
#endif // defined(_MSC_VER)

#elif defined(ARM) || defined(ARM64)

#include <arm_neon.h>

#endif // defined(_X86) || defined(_X64)

#ifdef __cplusplus
extern "C" {
#endif

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

#endif
