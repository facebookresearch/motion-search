
#include "moments.h"

#include <motion_search/inc/intrinsic.h>

#if defined(_X86) || defined(_X64)

int fastSAD16_sse2(FAST_SAD_FORMAL_ARGS)
{
    UNUSED(block_width);
    UNUSED(min_SAD);

    int i;
    int temp_SAD;
    __m128i xmm0, xmm1, xmm2;

    xmm2 = _mm_setzero_si128();
    for(i=block_height; i>0; i--)
    {
        xmm0 = _mm_load_si128((__m128i*) current);
        xmm1 = _mm_loadu_si128((__m128i*) reference);
        xmm0 = _mm_sad_epu8(xmm0, xmm1);

        xmm2 = _mm_adds_epu16(xmm2, xmm0);
        current += stride;
        reference += stride;
    }

    xmm0 = _mm_srli_si128(xmm2,8);
    xmm2 = _mm_adds_epu16(xmm2, xmm0);
    temp_SAD = _mm_cvtsi128_si32(xmm2);

    return temp_SAD;
}

int fastSAD8_sse2(FAST_SAD_FORMAL_ARGS)
{
    UNUSED(block_width);
    UNUSED(min_SAD);

    int i;
    int temp_SAD;
    __m128i xmm0, xmm1, xmm2;

    xmm2 = _mm_setzero_si128();
    for(i=block_height; i>0; i--)
    {
        xmm0 = _mm_loadl_epi64((const __m128i *) current);
        xmm1 = _mm_loadl_epi64((const __m128i *) reference);
        xmm0 = _mm_sad_epu8(xmm0, xmm1);

        xmm2 = _mm_adds_epu16(xmm2, xmm0);
        current += stride;
        reference += stride;
    }

    temp_SAD = _mm_cvtsi128_si32(xmm2);

    return temp_SAD;
}

int fastSAD4_sse2(FAST_SAD_FORMAL_ARGS)
{
    UNUSED(block_width);
    UNUSED(min_SAD);

    int i;
    int temp_SAD;
    __m128i xmm0, xmm1, xmm2;

    xmm2 = _mm_setzero_si128();
    for(i=block_height; i>0; i--)
    {
        xmm0 = _mm_loadu_si32(current);
        xmm1 = _mm_loadu_si32(reference);
        xmm0 = _mm_sad_epu8(xmm0, xmm1);

        xmm2 = _mm_adds_epu16(xmm2, xmm0);
        current += stride;
        reference += stride;
    }

    temp_SAD = _mm_cvtsi128_si32(xmm2);

    return temp_SAD;
}

int fast_variance16_sse2(FAST_VARIANCE_FORMAL_ARGS)
{
    UNUSED(block_width);

    int i;
    int temp, sum, sum2;
    __m128i xmm0, xmm1, xmm2, xmm4, xmm5;

    xmm2 = xmm5 = xmm4 = _mm_setzero_si128();
    for(i=block_height; i>0; i--)
    {
        xmm0 = _mm_load_si128((__m128i*) current);
        xmm1 = _mm_unpacklo_epi8(xmm0, xmm4);
        xmm0 = _mm_unpackhi_epi8(xmm0, xmm4);
        xmm2 = _mm_adds_epu16(xmm2, xmm1);
        xmm2 = _mm_adds_epu16(xmm2, xmm0);
        xmm1 = _mm_madd_epi16(xmm1, xmm1);
        xmm0 = _mm_madd_epi16(xmm0, xmm0);
        xmm5 = _mm_add_epi32(xmm5, xmm1);
        xmm5 = _mm_add_epi32(xmm5, xmm0);

        current += stride;
    }

    xmm0 = _mm_srli_epi64(xmm5, 4*8);
    xmm5 = _mm_add_epi32(xmm5, xmm0);
    xmm0 = _mm_srli_si128(xmm5, 8);
    xmm5 = _mm_add_epi32(xmm5, xmm0);
    sum2 = _mm_cvtsi128_si32(xmm5);

    xmm0 = _mm_srli_si128(xmm2, 8);
    xmm2 = _mm_adds_epu16(xmm2, xmm0);
    xmm0 = _mm_srli_si128(xmm2, 4);
    xmm2 = _mm_adds_epu16(xmm2, xmm0);
    xmm0 = _mm_srli_si128(xmm2, 2);
    xmm2 = _mm_adds_epu16(xmm2, xmm0);
    xmm2 = _mm_unpacklo_epi16(xmm2, xmm4);
    sum  = _mm_cvtsi128_si32(xmm2);

    temp = block_height<<4;
    return sum2 - (sum*sum+(temp>>1))/temp;
}

// Fast sum of square differences
int fast_variance8_sse2(FAST_VARIANCE_FORMAL_ARGS)
{
    UNUSED(block_width);

    int i;
    int temp, sum, sum2;
#if 0
    __m128i xmm0, xmm1, xmm2, xmm4, xmm5;

    xmm2 = xmm5 = xmm4 = _mm_setzero_si64();
    for(i=block_height; i>0; i--)
    {
        xmm0 = *((__m128i*) current);
        xmm1 = _mm_unpacklo_pi8(xmm0, xmm4);
        xmm0 = _mm_unpackhi_pi8(xmm0, xmm4);
        xmm2 = _mm_adds_pu16(xmm2, xmm1);
        xmm2 = _mm_adds_pu16(xmm2, xmm0);
        xmm1 = _mm_madd_pi16(xmm1, xmm1);
        xmm0 = _mm_madd_pi16(xmm0, xmm0);
        xmm5 = _mm_add_pi32(xmm5, xmm1);
        xmm5 = _mm_add_pi32(xmm5, xmm0);

        current += stride;
    }

    xmm0 = _mm_srli_si64(xmm5, 4*8);
    xmm5 = _mm_add_pi32(xmm5, xmm0);
    sum2 = _mm_cvtsi64_si32(xmm5);

    xmm0 = _mm_srli_si64(xmm2, 4*8);
    xmm2 = _mm_adds_pu16(xmm2, xmm0);
    xmm0 = _mm_srli_si64(xmm2, 2*8);
    xmm2 = _mm_adds_pu16(xmm2, xmm0);
    xmm2 = _mm_unpacklo_pi16(xmm2, xmm4);
    sum  = _mm_cvtsi64_si32(xmm2);
#else
    __m128i xmm0, xmm2, xmm4, xmm5;

    xmm2 = xmm5 = xmm4 = _mm_setzero_si128();
    for(i=block_height; i>0; i--)
    {
        xmm0 = _mm_loadl_epi64((__m128i*) current);
        xmm0 = _mm_unpacklo_epi8(xmm0, xmm4);
        xmm2 = _mm_adds_epu16(xmm2, xmm0);
        xmm0 = _mm_madd_epi16(xmm0, xmm0);
        xmm5 = _mm_add_epi32(xmm5, xmm0);

        current += stride;
    }

    xmm0 = _mm_srli_epi64(xmm5, 4*8);
    xmm5 = _mm_add_epi32(xmm5, xmm0);
    xmm0 = _mm_srli_si128(xmm5, 8);
    xmm5 = _mm_add_epi32(xmm5, xmm0);
    sum2 = _mm_cvtsi128_si32(xmm5);

    xmm0 = _mm_srli_si128(xmm2, 8);
    xmm2 = _mm_adds_epu16(xmm2, xmm0);
    xmm0 = _mm_srli_si128(xmm2, 4);
    xmm2 = _mm_adds_epu16(xmm2, xmm0);
    xmm0 = _mm_srli_si128(xmm2, 2);
    xmm2 = _mm_adds_epu16(xmm2, xmm0);
    xmm2 = _mm_unpacklo_epi16(xmm2, xmm4);
    sum  = _mm_cvtsi128_si32(xmm2);
#endif

    temp = block_height<<3;
    return sum2 - (sum*sum+(temp>>1))/temp;
}

// Fast sum of square differences
int fast_variance4_sse2(FAST_VARIANCE_FORMAL_ARGS)
{
    UNUSED(block_width);

    int i;
    int temp, sum, sum2;
    __m128i xmm0, xmm1, xmm2, xmm3;

    xmm1 = xmm2 = xmm3 = _mm_setzero_si128();
    for(i=block_height; i>0; i--)
    {
        xmm0 = _mm_loadu_si32(current);
        xmm0 = _mm_unpacklo_epi8(xmm0, xmm3);
        xmm1 = _mm_adds_epu16(xmm1, xmm0);
        xmm0 = _mm_madd_epi16(xmm0, xmm0);
        xmm2 = _mm_add_epi32(xmm2, xmm0);

        current += stride;
    }

    xmm0 = _mm_srli_epi64(xmm2, 4*8);
    xmm2 = _mm_add_epi32(xmm2, xmm0);
    sum2 = _mm_cvtsi128_si32(xmm2);

    xmm0 = _mm_srli_epi64(xmm1, 4*8);
    xmm1 = _mm_adds_epu16(xmm1, xmm0);
    xmm0 = _mm_srli_epi64(xmm1, 2*8);
    xmm1 = _mm_adds_epu16(xmm1, xmm0);
    xmm1 = _mm_unpacklo_epi16(xmm1, xmm3);
    sum  = _mm_cvtsi128_si32(xmm1);

    temp = block_height<<2;
    return sum2 - (sum*sum+(temp>>1))/temp;
}

// Fast sum of square differences
int fast_calc_mse16_sse2(FAST_MSE_FORMAL_ARGS)
{
    UNUSED(block_width);

    int i;
    int sum2;
    __m128i xmm0, xmm1, xmm2, xmm3, xmm4, xmm5;
#ifdef AC_ENERGY
    __m128i xmm6;
    int sum;
    int temp;
#endif // AC_ENERGY

    xmm5 = xmm4 = _mm_setzero_si128();
#ifdef AC_ENERGY
    xmm6 = _mm_setzero_si128();
#endif // AC_ENERGY
    for(i=block_height; i>0; i--)
    {
        xmm0 = _mm_load_si128((__m128i*) current);
        xmm1 = _mm_loadu_si128((__m128i*) reference);
        xmm2 = _mm_unpacklo_epi8(xmm0, xmm4);
        xmm0 = _mm_unpackhi_epi8(xmm0, xmm4);
        xmm3 = _mm_unpacklo_epi8(xmm1, xmm4);
        xmm1 = _mm_unpackhi_epi8(xmm1, xmm4);
        xmm2 = _mm_sub_epi16(xmm2, xmm3);
        xmm0 = _mm_sub_epi16(xmm0, xmm1);
#ifdef AC_ENERGY
        xmm6 = _mm_adds_epi16(xmm6, xmm2);
        xmm6 = _mm_adds_epi16(xmm6, xmm0);
#endif // AC_ENERGY
        xmm2 = _mm_madd_epi16(xmm2, xmm2);
        xmm0 = _mm_madd_epi16(xmm0, xmm0);
        xmm5 = _mm_add_epi32(xmm5, xmm2);
        xmm5 = _mm_add_epi32(xmm5, xmm0);

        current += stride;
        reference += stride;
    }

    xmm0 = _mm_srli_epi64(xmm5, 4*8);
    xmm5 = _mm_add_epi32(xmm5, xmm0);
    xmm0 = _mm_srli_si128(xmm5, 8);
    xmm5 = _mm_add_epi32(xmm5, xmm0);
    sum2 = _mm_cvtsi128_si32(xmm5);

#ifdef AC_ENERGY
    xmm0 = _mm_srli_epi32(xmm6, 1*16);
    xmm6 = _mm_adds_epi16(xmm6, xmm0);
    xmm0 = _mm_srli_epi64(xmm6, 2*16);
    xmm6 = _mm_adds_epi16(xmm6, xmm0);
    xmm6 = _mm_slli_epi32(xmm6, 1*16);
    xmm6 = _mm_srai_epi32(xmm6, 1*16);
    xmm0 = _mm_srli_si128(xmm6, 8);
    xmm6 = _mm_add_epi32(xmm6, xmm0);
    sum = _mm_cvtsi128_si32(xmm6);

    temp = block_height<<4;
    sum2 -= (sum*sum+(temp>>1))/temp;
#endif // AC_ENERGY
    return sum2;
}

// Fast sum of square differences
int fast_calc_mse8_sse2(FAST_MSE_FORMAL_ARGS)
{
    UNUSED(block_width);

    int i;
    int sum2;
    __m128i xmm0, xmm1, xmm2, xmm3, xmm4, xmm5;
#ifdef AC_ENERGY
    __m128i xmm6;
    int sum;
    int temp;
#endif // AC_ENERGY

    xmm5 = xmm4 = _mm_setzero_si128();
#ifdef AC_ENERGY
    xmm6 = _mm_setzero_si128();
#endif // AC_ENERGY
    for(i=block_height; i>0; i--)
    {
        xmm0 = _mm_loadl_epi64((__m128i *) current);
        xmm1 = _mm_loadl_epi64((__m128i *) reference);
        xmm2 = _mm_unpacklo_epi8(xmm0, xmm4);
        xmm0 = _mm_unpackhi_epi8(xmm0, xmm4);
        xmm3 = _mm_unpacklo_epi8(xmm1, xmm4);
        xmm1 = _mm_unpackhi_epi8(xmm1, xmm4);
        xmm2 = _mm_sub_epi16(xmm2, xmm3);
        xmm0 = _mm_sub_epi16(xmm0, xmm1);
#ifdef AC_ENERGY
        xmm6 = _mm_adds_epi16(xmm6, xmm2);
        xmm6 = _mm_adds_epi16(xmm6, xmm0);
#endif // AC_ENERGY
        xmm2 = _mm_madd_epi16(xmm2, xmm2);
        xmm0 = _mm_madd_epi16(xmm0, xmm0);
        xmm5 = _mm_add_epi32(xmm5, xmm2);
        xmm5 = _mm_add_epi32(xmm5, xmm0);

        current += stride;
        reference += stride;
    }

    xmm0 = _mm_srli_epi64(xmm5, 4*8);
    xmm5 = _mm_add_epi32(xmm5, xmm0);
    sum2 = _mm_cvtsi128_si32(xmm5);

#ifdef AC_ENERGY
    xmm0 = _mm_srli_epi32(xmm6, 1*16);
    xmm6 = _mm_adds_epi16(xmm6, xmm0);
    xmm0 = _mm_srli_epi64(xmm6, 2*16);
    xmm6 = _mm_adds_epi16(xmm6, xmm0);
    xmm6 = _mm_slli_epi32(xmm6, 1*16);
    xmm6 = _mm_srai_epi32(xmm6, 1*16);
    sum = _mm_cvtsi128_si32(xmm6);

    temp = block_height<<3;
    sum2 -= (sum*sum+(temp>>1))/temp;
#endif // AC_ENERGY
    return sum2;
}


// Fast sum of square differences
int fast_calc_mse4_sse2(FAST_MSE_FORMAL_ARGS)
{
    UNUSED(block_width);

    int i;
    int sum2;
    __m128i xmm0, xmm1, xmm2, xmm3;
#ifdef AC_ENERGY
    __m128i xmm6;
    int sum;
    int temp;
#endif // AC_ENERGY

    xmm2 = xmm3 = _mm_setzero_si128();
#ifdef AC_ENERGY
    xmm6 = _mm_setzero_si128();
#endif // AC_ENERGY
    for(i=block_height; i>0; i--)
    {
        xmm0 = _mm_loadu_si32(current);
        xmm1 = _mm_loadu_si32(reference);
        xmm0 = _mm_unpacklo_epi8(xmm0, xmm3);
        xmm1 = _mm_unpacklo_epi8(xmm1, xmm3);
        xmm0 = _mm_sub_epi16(xmm0, xmm1);
#ifdef AC_ENERGY
        xmm6 = _mm_adds_epi16(xmm6, xmm0);
#endif // AC_ENERGY
        xmm0 = _mm_madd_epi16(xmm0, xmm0);
        xmm2 = _mm_add_epi32(xmm2, xmm0);

        current += stride;
        reference += stride;
    }

    xmm0 = _mm_srli_epi64(xmm2, 4*8);
    xmm2 = _mm_add_epi32(xmm2, xmm0);
    sum2 = _mm_cvtsi128_si32(xmm2);

#ifdef AC_ENERGY
    xmm0 = _mm_srli_epi32(xmm6, 1*16);
    xmm6 = _mm_adds_epi16(xmm6, xmm0);
    xmm0 = _mm_srli_epi64(xmm6, 2*16);
    xmm6 = _mm_adds_epi16(xmm6, xmm0);
    xmm6 = _mm_slli_epi32(xmm6, 1*16);
    xmm6 = _mm_srai_epi32(xmm6, 1*16);
    sum = _mm_cvtsi128_si32(xmm6);

    temp = block_height<<2;
    sum2 -= (sum*sum+(temp>>1))/temp;
#endif // AC_ENERGY
    return sum2;
}

// Fast sum of square differences after bi-directional interpolation; we assume td1+td2 = 32768
int fast_bidir_mse16_sse2(FAST_BIDIR_MSE_FORMAL_ARGS)
{
    UNUSED(block_width);

    static const __declspec(align(16)) int constant_16384[4] = {16384, 16384, 16384, 16384};
    int i;
    int sum2;
#ifdef AC_ENERGY
    int temp;
    int sum;
#endif // AC_ENERGY
    __m128i xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;

    xmm6 = xmm7 = _mm_setzero_si128();
    xmm5 = _mm_cvtsi32_si128(*(int *) td);
    xmm5 = _mm_shuffle_epi32(xmm5, 0);
    for(i=block_height; i>0; i--)
    {
        xmm0 = _mm_loadu_si128((__m128i*) reference1);
        xmm1 = _mm_loadu_si128((__m128i*) reference2);
#ifdef AC_ENERGY
        xmm4 = _mm_setzero_si128();
        xmm2 = _mm_unpacklo_epi8(xmm0, xmm4);
        xmm0 = _mm_unpackhi_epi8(xmm0, xmm4);
        xmm3 = _mm_unpacklo_epi8(xmm1, xmm4);
        xmm1 = _mm_unpackhi_epi8(xmm1, xmm4);
#else
        xmm2 = _mm_unpacklo_epi8(xmm0, xmm6);
        xmm0 = _mm_unpackhi_epi8(xmm0, xmm6);
        xmm3 = _mm_unpacklo_epi8(xmm1, xmm6);
        xmm1 = _mm_unpackhi_epi8(xmm1, xmm6);
#endif // AC_ENERGY
        xmm4 = _mm_unpacklo_epi16(xmm2, xmm3);
        xmm2 = _mm_unpackhi_epi16(xmm2, xmm3);
        xmm3 = _mm_unpacklo_epi16(xmm0, xmm1);
        xmm0 = _mm_unpackhi_epi16(xmm0, xmm1);
        xmm4 = _mm_madd_epi16(xmm4, xmm5);
        xmm2 = _mm_madd_epi16(xmm2, xmm5);
        xmm3 = _mm_madd_epi16(xmm3, xmm5);
        xmm0 = _mm_madd_epi16(xmm0, xmm5);
        xmm1 = _mm_load_si128((__m128i*) constant_16384);
        xmm4 = _mm_add_epi32(xmm4, xmm1);
        xmm2 = _mm_add_epi32(xmm2, xmm1);
        xmm3 = _mm_add_epi32(xmm3, xmm1);
        xmm0 = _mm_add_epi32(xmm0, xmm1);
        xmm4 = _mm_srli_epi32(xmm4, 15);
        xmm2 = _mm_srli_epi32(xmm2, 15);
        xmm3 = _mm_srli_epi32(xmm3, 15);
        xmm0 = _mm_srli_epi32(xmm0, 15);
        xmm4 = _mm_packs_epi32(xmm4, xmm2);
        xmm3 = _mm_packs_epi32(xmm3, xmm0);

        xmm1 = _mm_load_si128((__m128i*) current);
#ifdef AC_ENERGY
        xmm0 = _mm_setzero_si128();
        xmm2 = _mm_unpacklo_epi8(xmm1, xmm0);
        xmm1 = _mm_unpackhi_epi8(xmm1, xmm0);
#else
        xmm2 = _mm_unpacklo_epi8(xmm1, xmm6);
        xmm1 = _mm_unpackhi_epi8(xmm1, xmm6);
#endif // AC_ENERGY
        xmm4 = _mm_sub_epi16(xmm4, xmm2);
        xmm3 = _mm_sub_epi16(xmm3, xmm1);
#ifdef AC_ENERGY
        xmm6 = _mm_adds_epi16(xmm6, xmm4);
        xmm6 = _mm_adds_epi16(xmm6, xmm3);
#endif // AC_ENERGY
        xmm4 = _mm_madd_epi16(xmm4, xmm4);
        xmm3 = _mm_madd_epi16(xmm3, xmm3);
        xmm7 = _mm_add_epi32(xmm7, xmm4);
        xmm7 = _mm_add_epi32(xmm7, xmm3);

        current += stride;
        reference1 += stride;
        reference2 += stride;
    }

    xmm0 = _mm_srli_epi64(xmm7, 4*8);
    xmm7 = _mm_add_epi32(xmm7, xmm0);
    xmm0 = _mm_srli_si128(xmm7, 8);
    xmm7 = _mm_add_epi32(xmm7, xmm0);
    sum2 = _mm_cvtsi128_si32(xmm7);

#ifdef AC_ENERGY
    xmm0 = _mm_srli_epi32(xmm6, 1*16);
    xmm6 = _mm_adds_epi16(xmm6, xmm0);
    xmm0 = _mm_srli_epi64(xmm6, 2*16);
    xmm6 = _mm_adds_epi16(xmm6, xmm0);
    xmm6 = _mm_slli_epi32(xmm6, 1*16);
    xmm6 = _mm_srai_epi32(xmm6, 1*16);
    xmm0 = _mm_srli_si128(xmm6, 8);
    xmm6 = _mm_add_epi32(xmm6, xmm0);
    sum = _mm_cvtsi128_si32(xmm6);

    temp = block_height<<4;
    sum2 -= (sum*sum+(temp>>1))/temp;
#endif // AC_ENERGY
    return sum2;
}

// Fast sum of square differences after bi-directional interpolation; we assume td1+td2 = 32768
int fast_bidir_mse8_sse2(FAST_BIDIR_MSE_FORMAL_ARGS)
{
    UNUSED(block_width);

    static const __declspec(align(16)) int constant_16384[4] = {16384, 16384, 16384, 16384};
    int i;
    int sum2;
#ifdef AC_ENERGY
    int temp;
    int sum;
#endif // AC_ENERGY
    __m128i xmm0, xmm1, xmm2, xmm4, xmm5, xmm6, xmm7;
#ifdef AC_ENERGY
    __m128i xmm3;
#endif // AC_ENERGY

    xmm6 = xmm7 = _mm_setzero_si128();
#ifdef AC_ENERGY
    xmm3 = _mm_setzero_si128();
#endif // AC_ENERGY
    xmm5 = _mm_cvtsi32_si128(*(int *) td);
    xmm5 = _mm_shuffle_epi32(xmm5, 0);
    xmm4 = _mm_load_si128((__m128i*) constant_16384);
    for(i=block_height; i>0; i--)
    {
        xmm0 = _mm_loadl_epi64((__m128i*) reference1);
        xmm1 = _mm_loadl_epi64((__m128i*) reference2);
        xmm0 = _mm_unpacklo_epi8(xmm0, xmm6);
        xmm1 = _mm_unpacklo_epi8(xmm1, xmm6);
        xmm2 = _mm_unpacklo_epi16(xmm0, xmm1);
        xmm0 = _mm_unpackhi_epi16(xmm0, xmm1);
        xmm2 = _mm_madd_epi16(xmm2, xmm5);
        xmm0 = _mm_madd_epi16(xmm0, xmm5);
        xmm2 = _mm_add_epi32(xmm2, xmm4);
        xmm0 = _mm_add_epi32(xmm0, xmm4);
        xmm2 = _mm_srli_epi32(xmm2, 15);
        xmm0 = _mm_srli_epi32(xmm0, 15);
        xmm2 = _mm_packs_epi32(xmm2, xmm0);

        xmm1 = _mm_loadl_epi64((__m128i*) current);
        xmm1 = _mm_unpacklo_epi8(xmm1, xmm6);
        xmm2 = _mm_sub_epi16(xmm2, xmm1);
#ifdef AC_ENERGY
        xmm3 = _mm_adds_epi16(xmm3, xmm2);
#endif // AC_ENERGY
        xmm2 = _mm_madd_epi16(xmm2, xmm2);
        xmm7 = _mm_add_epi32(xmm7, xmm2);

        current += stride;
        reference1 += stride;
        reference2 += stride;
    }

    xmm0 = _mm_srli_epi64(xmm7, 4*8);
    xmm7 = _mm_add_epi32(xmm7, xmm0);
    xmm0 = _mm_srli_si128(xmm7, 8);
    xmm7 = _mm_add_epi32(xmm7, xmm0);
    sum2 = _mm_cvtsi128_si32(xmm7);

#ifdef AC_ENERGY
    xmm0 = _mm_srli_epi32(xmm3, 1*16);
    xmm3 = _mm_adds_epi16(xmm3, xmm0);
    xmm0 = _mm_srli_epi64(xmm3, 2*16);
    xmm3 = _mm_adds_epi16(xmm3, xmm0);
    xmm3 = _mm_slli_epi32(xmm3, 1*16);
    xmm3 = _mm_srai_epi32(xmm3, 1*16);
    xmm0 = _mm_srli_si128(xmm3, 8);
    xmm3 = _mm_add_epi32(xmm3, xmm0);
    sum = _mm_cvtsi128_si32(xmm3);

    temp = block_height<<3;
    sum2 -= (sum*sum+(temp>>1))/temp;
#endif // AC_ENERGY
    return sum2;
}

// Fast sum of square differences after bi-directional interpolation; we assume td1+td2 = 32768
int fast_bidir_mse4_sse2(FAST_BIDIR_MSE_FORMAL_ARGS)
{
    UNUSED(block_width);

    static const __declspec(align(16)) int constant_16384[4] = {16384, 16384, 16384, 16384};
    int i;
    int sum2;
#ifdef AC_ENERGY
    int temp;
    int sum;
#endif // AC_ENERGY
    __m128i xmm0, xmm1, xmm4, xmm5, xmm6, xmm7;
#ifdef AC_ENERGY
    __m128i xmm3;
#endif // AC_ENERGY

    xmm6 = xmm7 = _mm_setzero_si128();
#ifdef AC_ENERGY
    xmm3 = _mm_setzero_si128();
#endif // AC_ENERGY
    xmm5 = _mm_cvtsi32_si128(*(int *) td);
    xmm5 = _mm_shuffle_epi32(xmm5, 0);
    xmm4 = _mm_load_si128((__m128i*) constant_16384);
    for(i=block_height; i>0; i--)
    {
        xmm0 = _mm_cvtsi32_si128(*(int *) reference1);
        xmm1 = _mm_cvtsi32_si128(*(int *) reference2);
        xmm0 = _mm_unpacklo_epi8(xmm0, xmm6);
        xmm1 = _mm_unpacklo_epi8(xmm1, xmm6);
        xmm0 = _mm_unpacklo_epi16(xmm0, xmm1);
        xmm0 = _mm_madd_epi16(xmm0, xmm5);
        xmm0 = _mm_add_epi32(xmm0, xmm4);
        xmm0 = _mm_srli_epi32(xmm0, 15);
        xmm0 = _mm_packs_epi32(xmm0, xmm6);

        xmm1 = _mm_cvtsi32_si128(*(int *) current);
        xmm1 = _mm_unpacklo_epi8(xmm1, xmm6);
        xmm0 = _mm_sub_epi16(xmm0, xmm1);
#ifdef AC_ENERGY
        xmm3 = _mm_adds_epi16(xmm3, xmm0);
#endif // AC_ENERGY
        xmm0 = _mm_madd_epi16(xmm0, xmm0);
        xmm7 = _mm_add_epi32(xmm7, xmm0);

        current += stride;
        reference1 += stride;
        reference2 += stride;
    }

    xmm0 = _mm_srli_epi64(xmm7, 4*8);
    xmm7 = _mm_add_epi32(xmm7, xmm0);
    xmm0 = _mm_srli_si128(xmm7, 8);
    xmm7 = _mm_add_epi32(xmm7, xmm0);
    sum2 = _mm_cvtsi128_si32(xmm7);

#ifdef AC_ENERGY
    xmm0 = _mm_srli_epi32(xmm3, 1*16);
    xmm3 = _mm_adds_epi16(xmm3, xmm0);
    xmm0 = _mm_srli_epi64(xmm3, 2*16);
    xmm3 = _mm_adds_epi16(xmm3, xmm0);
    xmm3 = _mm_slli_epi32(xmm3, 1*16);
    xmm3 = _mm_srai_epi32(xmm3, 1*16);
    sum = _mm_cvtsi128_si32(xmm3);

    temp = block_height<<2;
    sum2 -= (sum*sum+(temp>>1))/temp;
#endif // AC_ENERGY
    return sum2;
}

#endif // defined(_X86) || defined(_X64)
