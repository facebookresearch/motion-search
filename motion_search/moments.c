#include "moments.h"

// The following is a warning about no EMMS instruction for a function that uses MMX instructions
// Disabled, since we don't use any float or double operations.
#pragma warning(disable: 4799)

// WxH SAD
int fullSAD(unsigned char *current, unsigned char *reference, int stride, int block_width, int block_height, int min_SAD)
{
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

// WxH SAD using early termination
int partialSAD(unsigned char *current, unsigned char *reference, int stride, int block_width, int block_height, int min_SAD)
{
	int temp_SAD;
	int diff;
	int i, j;

	temp_SAD = min_SAD;
	for(i=block_height;i>0 && temp_SAD>0;i--)
	{
		for(j=0;j<block_width;j++)
		{
			diff = abs(current[j]-reference[j]);
			temp_SAD -= diff;
		}
		current += stride;
		reference += stride;
	}

	return min_SAD-temp_SAD;
}

#if defined(USE_MMX_SSE)

// 16xH SAD using SSE2 instructions
int fastSAD16(unsigned char *current, unsigned char *reference, int stride, int block_width, int block_height, int min_SAD)
{
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

// 8xH SAD using MMX instructions
int fastSAD8(unsigned char *current, unsigned char *reference, int stride, int block_width, int block_height, int min_SAD)
{
	int i;
	int temp_SAD;
	__m64 mm0, mm1, mm2;

	mm2 = _mm_setzero_si64();
	for(i=block_height; i>0; i--)
	{
		mm0 = *(__m64*) current;
		mm1 = *(__m64*) reference;
		mm0 = _mm_sad_pu8(mm0, mm1);

		mm2 = _mm_adds_pu16(mm2, mm0);
		current += stride;
		reference += stride;
	}

	temp_SAD = _mm_cvtsi64_si32(mm2);

	return temp_SAD;
}

// 4xH SAD using MMX instructions
int fastSAD4(unsigned char *current, unsigned char *reference, int stride, int block_width, int block_height, int min_SAD)
{
	int i;
	int temp_SAD;
	__m64 mm0, mm1, mm2;

	mm2 = _mm_setzero_si64();
	for(i=block_height; i>0; i--)
	{
		mm0 =  _mm_cvtsi32_si64(*(unsigned int *) current);
		mm1 =  _mm_cvtsi32_si64(*(unsigned int *) reference);
		mm0 = _mm_sad_pu8(mm0, mm1);

		mm2 = _mm_adds_pu16(mm2, mm0);
		current += stride;
		reference += stride;
	}

	temp_SAD = _mm_cvtsi64_si32(mm2);

	return temp_SAD;
}

#else

#define fastSAD16 partialSAD
#define fastSAD8 partialSAD
#define fastSAD4 partialSAD

#endif // USE_MMX_SSE

// Return block variance, multiplied by block_width*block_height
int variance(unsigned char *current, int stride, int block_width, int block_height)
{
	int i, j;
	int temp;
	int sum;
	int sum2;

	sum = sum2 = 0;
	for(i=block_height;i>0;i--)
	{
		for(j=0;j<block_width;j++)
		{
			temp = current[j];
			sum += temp;
			sum2 += temp*temp;
		}
		current += stride;
	}

	temp = block_height*block_width;
	return sum2 - (sum*sum+(temp>>1))/temp;
}

#if defined(USE_MMX_SSE)

// Fast sum of square differences
int fast_variance16(unsigned char *current, int stride, int block_width, int block_height)
{
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
int fast_variance8(unsigned char *current, int stride, int block_width, int block_height)
{
	int i;
	int temp, sum, sum2;
#if 0
	__m64 mm0, mm1, mm2, mm4, mm5;

	mm2 = mm5 = mm4 = _mm_setzero_si64();
	for(i=block_height; i>0; i--)
	{
		mm0 = *((__m64*) current);
		mm1 = _mm_unpacklo_pi8(mm0, mm4);
		mm0 = _mm_unpackhi_pi8(mm0, mm4);
		mm2 = _mm_adds_pu16(mm2, mm1);
		mm2 = _mm_adds_pu16(mm2, mm0);
		mm1 = _mm_madd_pi16(mm1, mm1);
		mm0 = _mm_madd_pi16(mm0, mm0);
		mm5 = _mm_add_pi32(mm5, mm1);
		mm5 = _mm_add_pi32(mm5, mm0);

		current += stride;
	}

	mm0 = _mm_srli_si64(mm5, 4*8);
	mm5 = _mm_add_pi32(mm5, mm0);
	sum2 = _mm_cvtsi64_si32(mm5);

	mm0 = _mm_srli_si64(mm2, 4*8);
	mm2 = _mm_adds_pu16(mm2, mm0);
	mm0 = _mm_srli_si64(mm2, 2*8);
	mm2 = _mm_adds_pu16(mm2, mm0);
	mm2 = _mm_unpacklo_pi16(mm2, mm4);
	sum  = _mm_cvtsi64_si32(mm2);
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
int fast_variance4(unsigned char *current, int stride, int block_width, int block_height)
{
	int i;
	int temp, sum, sum2;
	__m64 mm0, mm1, mm2, mm3;

	mm1 = mm2 = mm3 = _mm_setzero_si64();
	for(i=block_height; i>0; i--)
	{
		mm0 = _mm_cvtsi32_si64(*(unsigned int *) current);
		mm0 = _mm_unpacklo_pi8(mm0, mm3);
		mm1 = _mm_adds_pu16(mm1, mm0);
		mm0 = _mm_madd_pi16(mm0, mm0);
		mm2 = _mm_add_pi32(mm2, mm0);

		current += stride;
	}

	mm0 = _mm_srli_si64(mm2, 4*8);
	mm2 = _mm_add_pi32(mm2, mm0);
	sum2 = _mm_cvtsi64_si32(mm2);

	mm0 = _mm_srli_si64(mm1, 4*8);
	mm1 = _mm_adds_pu16(mm1, mm0);
	mm0 = _mm_srli_si64(mm1, 2*8);
	mm1 = _mm_adds_pu16(mm1, mm0);
	mm1 = _mm_unpacklo_pi16(mm1, mm3);
	sum  = _mm_cvtsi64_si32(mm1);

	temp = block_height<<2;
	return sum2 - (sum*sum+(temp>>1))/temp;
}

#else

#define fast_variance16 variance
#define fast_variance8 variance
#define fast_variance4 variance

#endif // USE_MMX_SSE

// Sum of square differences
int calc_mse(unsigned char *current, unsigned char *reference, int stride, int block_width, int block_height)
{
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
	for(i=block_height;i>0;i--)
	{
		for(j=0;j<block_width;j++)
		{
			temp = current[j]-reference[j];
#ifdef AC_ENERGY
			sum  += temp;
#endif // AC_ENERGY
			sum2 += temp*temp;
		}
		current += stride;
		reference += stride;
	}

#ifdef AC_ENERGY
	temp = block_height*block_width;
	sum2 -= (sum*sum+(temp>>1))/temp;
#endif // AC_ENERGY
	return sum2;
}

#if defined(USE_MMX_SSE)

// Fast sum of square differences
int fast_calc_mse16(unsigned char *current, unsigned char *reference, int stride, int block_width, int block_height)
{
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
int fast_calc_mse8(unsigned char *current, unsigned char *reference, int stride, int block_width, int block_height)
{
	int i;
	int sum2;
	__m64 mm0, mm1, mm2, mm3, mm4, mm5;
#ifdef AC_ENERGY
	__m64 mm6;
	int sum;
	int temp;
#endif // AC_ENERGY

	mm5 = mm4 = _mm_setzero_si64();
#ifdef AC_ENERGY
	mm6 = _mm_setzero_si64();
#endif // AC_ENERGY
	for(i=block_height; i>0; i--)
	{
		mm0 = *((__m64*) current);
		mm1 = *((__m64*) reference);
		mm2 = _mm_unpacklo_pi8(mm0, mm4);
		mm0 = _mm_unpackhi_pi8(mm0, mm4);
		mm3 = _mm_unpacklo_pi8(mm1, mm4);
		mm1 = _mm_unpackhi_pi8(mm1, mm4);
		mm2 = _mm_sub_pi16(mm2, mm3);
		mm0 = _mm_sub_pi16(mm0, mm1);
#ifdef AC_ENERGY
		mm6 = _mm_adds_pi16(mm6, mm2);
		mm6 = _mm_adds_pi16(mm6, mm0);
#endif // AC_ENERGY
		mm2 = _mm_madd_pi16(mm2, mm2);
		mm0 = _mm_madd_pi16(mm0, mm0);
		mm5 = _mm_add_pi32(mm5, mm2);
		mm5 = _mm_add_pi32(mm5, mm0);

		current += stride;
		reference += stride;
	}

	mm0 = _mm_srli_si64(mm5, 4*8);
	mm5 = _mm_add_pi32(mm5, mm0);
	sum2 = _mm_cvtsi64_si32(mm5);

#ifdef AC_ENERGY
	mm0 = _mm_srli_pi32(mm6, 1*16);
	mm6 = _mm_adds_pi16(mm6, mm0);
	mm0 = _mm_srli_si64(mm6, 2*16);
	mm6 = _mm_adds_pi16(mm6, mm0);
	mm6 = _mm_slli_pi32(mm6, 1*16);
	mm6 = _mm_srai_pi32(mm6, 1*16);
	sum = _mm_cvtsi64_si32(mm6);

	temp = block_height<<3;
	sum2 -= (sum*sum+(temp>>1))/temp;
#endif // AC_ENERGY
	return sum2;
}


// Fast sum of square differences
int fast_calc_mse4(unsigned char *current, unsigned char *reference, int stride, int block_width, int block_height)
{
	int i;
	int sum2;
	__m64 mm0, mm1, mm2, mm3;
#ifdef AC_ENERGY
	__m64 mm6;
	int sum;
	int temp;
#endif // AC_ENERGY

	mm2 = mm3 = _mm_setzero_si64();
#ifdef AC_ENERGY
	mm6 = _mm_setzero_si64();
#endif // AC_ENERGY
	for(i=block_height; i>0; i--)
	{
		mm0 =  _mm_cvtsi32_si64(*(unsigned int *) current);
		mm1 =  _mm_cvtsi32_si64(*(unsigned int *) reference);
		mm0 = _mm_unpacklo_pi8(mm0, mm3);
		mm1 = _mm_unpacklo_pi8(mm1, mm3);
		mm0 = _mm_sub_pi16(mm0, mm1);
#ifdef AC_ENERGY
		mm6 = _mm_adds_pi16(mm6, mm0);
#endif // AC_ENERGY
		mm0 = _mm_madd_pi16(mm0, mm0);
		mm2 = _mm_add_pi32(mm2, mm0);

		current += stride;
		reference += stride;
	}

	mm0 = _mm_srli_si64(mm2, 4*8);
	mm2 = _mm_add_pi32(mm2, mm0);
	sum2 = _mm_cvtsi64_si32(mm2);

#ifdef AC_ENERGY
	mm0 = _mm_srli_pi32(mm6, 1*16);
	mm6 = _mm_adds_pi16(mm6, mm0);
	mm0 = _mm_srli_si64(mm6, 2*16);
	mm6 = _mm_adds_pi16(mm6, mm0);
	mm6 = _mm_slli_pi32(mm6, 1*16);
	mm6 = _mm_srai_pi32(mm6, 1*16);
	sum = _mm_cvtsi64_si32(mm6);

	temp = block_height<<2;
	sum2 -= (sum*sum+(temp>>1))/temp;
#endif // AC_ENERGY
	return sum2;
}

#else

#define fast_calc_mse16 calc_mse
#define fast_calc_mse8 calc_mse
#define fast_calc_mse4 calc_mse

#endif // USE_MMX_SSE

// Sum of square differences after bi-directional interpolation; we assume td1+td2 = 32768
int bidir_mse(unsigned char *current, unsigned char *reference1, unsigned char *reference2, int stride, int block_width, int block_height, MV *td)
{
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
	for(i=block_height;i>0;i--)
	{
		for(j=0;j<block_width;j++)
		{
			temp = current[j] - ((reference1[j]*(td->y)+reference2[j]*(td->x)+16384)>>15);
#ifdef AC_ENERGY
			sum  += temp;
#endif // AC_ENERGY
			sum2 += temp*temp;
		}
		current += stride;
		reference1 += stride;
		reference2 += stride;
	}

#ifdef AC_ENERGY
	temp = block_height*block_width;
	sum2 -= (sum*sum+(temp>>1))/temp;
#endif // AC_ENERGY
	return sum2;
}

#if defined(USE_MMX_SSE)

// Fast sum of square differences after bi-directional interpolation; we assume td1+td2 = 32768
int fast_bidir_mse16(unsigned char *current, unsigned char *reference1, unsigned char *reference2, int stride, int block_width, int block_height, MV *td)
{
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
int fast_bidir_mse8(unsigned char *current, unsigned char *reference1, unsigned char *reference2, int stride, int block_width, int block_height, MV *td)
{
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
int fast_bidir_mse4(unsigned char *current, unsigned char *reference1, unsigned char *reference2, int stride, int block_width, int block_height, MV *td)
{
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

#else

#define fast_bidir_mse16 bidir_mse
#define fast_bidir_mse8 bidir_mse
#define fast_bidir_mse4 bidir_mse

#endif // USE_MMX_SSE
