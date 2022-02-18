#include "common.h"
#include "moments.h"
#include "frame.h"
#include "motion_search.h"

#define RANGE_CLIP(low, val, high) ((val) < (low) ? (low) : ((val) > (high) ? (high) : (val)))

typedef union _offt
{
	MV mv;
	short val[2];
}
diamond_offset_t;

typedef int (*t_SAD)(unsigned char *current, unsigned char *reference, int stride, int block_width, int block_height, int min_SAD);

static int diamond_search(unsigned char *current, unsigned char *reference, int stride, MV *motion_vector, int block_width, int block_height, const diamond_offset_t *offset, const int search_size, int min_SAD, const diamond_offset_t *next_diamond,t_SAD SAD)
{
	// Large diamond has 9 search locations
	int SAD_val[9];
	int min_ind;
	int i, j;
	const diamond_offset_t *ptr = next_diamond;

	do
	{
		for(i=0;(j=ptr[i].val[0])!=0;i++)
		{
			SAD_val[j] = SAD_val[ptr[i].val[1]];
		}
		SAD_val[0] = min_SAD;
		min_ind = 0;
		for(i++;i<search_size;i++)
		{
			j = ptr[i].val[0];
			SAD_val[j] = SAD(current, reference+offset[j].mv.y*stride+offset[j].mv.x, stride, block_width, block_height, min_SAD);
			if(SAD_val[j]<min_SAD)
			{
				min_SAD = SAD_val[j];
				min_ind = j;
			}
		}
		motion_vector->y += offset[min_ind].mv.y;
		motion_vector->x += offset[min_ind].mv.x;
		reference += offset[min_ind].mv.y*stride+offset[min_ind].mv.x;
		ptr = &next_diamond[min_ind*search_size];
	}
	while(min_ind!=0);

	return min_SAD;
}

//find mean or median of predictors
static int calc_median(const MV *motion_vector1, const MV *motion_vector2, const MV *motion_vector3, MV *median)
{
	median->y = motion_vector1->y+motion_vector2->y+motion_vector3->y;
	median->x = motion_vector1->x+motion_vector2->x+motion_vector3->x;
#if defined(USE_MEAN)
	median->y = (median->y+1)/3;
	median->x = (median->x+1)/3;
#else
	if(motion_vector1->y<motion_vector2->y)
	{
		if(motion_vector1->y<motion_vector3->y)
		{
			median->y -= motion_vector1->y;
		}
		else
		{
			median->y -= motion_vector3->y;
		}
		if(motion_vector2->y<motion_vector3->y)
		{
			median->y -= motion_vector3->y;
		}
		else
		{
			median->y -= motion_vector2->y;
		}
	}
	else
	{
		if(motion_vector2->y<motion_vector3->y)
		{
			median->y -= motion_vector2->y;
		}
		else
		{
			median->y -= motion_vector3->y;
		}
		if(motion_vector1->y<motion_vector3->y)
		{
			median->y -= motion_vector3->y;
		}
		else
		{
			median->y -= motion_vector1->y;
		}
	}
	if(motion_vector1->x<motion_vector2->x)
	{
		if(motion_vector1->x<motion_vector3->x)
		{
			median->x -= motion_vector1->x;
		}
		else
		{
			median->x -= motion_vector3->x;
		}
		if(motion_vector2->x<motion_vector3->x)
		{
			median->x -= motion_vector3->x;
		}
		else
		{
			median->x -= motion_vector2->x;
		}
	}
	else
	{
		if(motion_vector2->x<motion_vector3->x)
		{
			median->x -= motion_vector2->x;
		}
		else
		{
			median->x -= motion_vector3->x;
		}
		if(motion_vector1->x<motion_vector3->x)
		{
			median->x -= motion_vector3->x;
		}
		else
		{
			median->x -= motion_vector1->x;
		}
	}
#endif
	return 1;
}

/*
  . x2  .
 x3 x0 x1
  . x4  .
*/
static const diamond_offset_t small_diamond[5] = {
	{ 0, 0}, { 0, 1}, {-1, 0}, { 0,-1}, { 1, 0}
};

static const diamond_offset_t next_small_diamond[5*5] = {
	{ 0, 0}, { 1, 1}, { 2, 2}, { 3, 3}, { 4, 4}, 
	{ 3, 0}, { 0, 1}, { 1, 1}, { 2, 2}, { 4, 4}, 
	{ 4, 0}, { 0, 2}, { 1, 1}, { 2, 2}, { 3, 3}, 
	{ 1, 0}, { 0, 3}, { 2, 2}, { 3, 3}, { 4, 4}, 
	{ 2, 0}, { 0, 4}, { 1, 1}, { 3, 3}, { 4, 4}
};

/*
  .  . x3  .  .
  . x4  . x2  .
 x5  . x0  . x1
  . x6  . x8  .
  .  . x7  .  .
*/
static const diamond_offset_t large_diamond[9] = {
	{ 0, 0}, { 0, 2}, {-1, 1}, {-2, 0}, {-1,-1}, { 0,-2}, { 1,-1}, { 2, 0}, { 1, 1}
};

static const diamond_offset_t next_large_diamond[9*9] = {
	{ 0, 0}, { 1, 1}, { 2, 2}, { 3, 3}, { 4, 4}, { 5, 5}, { 6, 6}, { 7, 7}, { 8, 8}, 
	{ 5, 0}, { 4, 2}, { 6, 8}, { 0, 1}, { 1, 1}, { 2, 2}, { 3, 3}, { 7, 7}, { 8, 8}, 
	{ 6, 0}, { 5, 4}, { 7, 8}, { 4, 3}, { 8, 1}, { 0, 2}, { 1, 1}, { 2, 2}, { 3, 3}, 
	{ 7, 0}, { 6, 4}, { 8, 2}, { 0, 3}, { 1, 1}, { 2, 2}, { 3, 3}, { 4, 4}, { 5, 5}, 
	{ 8, 0}, { 1, 2}, { 7, 6}, { 2, 3}, { 6, 5}, { 0, 4}, { 3, 3}, { 4, 4}, { 5, 5}, 
	{ 1, 0}, { 2, 4}, { 8, 6}, { 0, 5}, { 3, 3}, { 4, 4}, { 5, 5}, { 6, 6}, { 7, 7}, 
	{ 2, 0}, { 1, 8}, { 3, 4}, { 8, 7}, { 4, 5}, { 0, 6}, { 5, 5}, { 6, 6}, { 7, 7}, 
	{ 3, 0}, { 2, 8}, { 4, 6}, { 0, 7}, { 1, 1}, { 5, 5}, { 6, 6}, { 7, 7}, { 8, 8}, 
	{ 4, 0}, { 3, 2}, { 5, 6}, { 1, 2}, { 7, 6}, { 0, 8}, { 3, 3}, { 4, 4}, { 5, 5}
};

/*
  x4 x3 x2
  x5 x0 x1
  x6 x7 x8
*/
static const diamond_offset_t small_block[9] = {
	{ 0, 0}, { 0, 1}, {-1, 1}, {-1, 0}, {-1,-1}, { 0,-1}, { 1,-1}, { 1, 0}, { 1, 1}
};
static const diamond_offset_t next_small_block[9*9] = {
	{ 0, 0}, { 1, 1}, { 2, 2}, { 3, 3}, { 4, 4}, { 5, 5}, { 6, 6}, { 7, 7}, { 8, 8}, 
	{ 5, 0}, { 4, 3}, { 6, 7}, { 2, 3}, { 8, 7}, { 0, 1}, { 2, 2}, { 1, 1}, { 8, 8}, 
	{ 6, 0}, { 5, 3}, { 7, 1}, { 0, 2}, { 4, 4}, { 3, 3}, { 2, 2}, { 1, 1}, { 8, 8}, 
	{ 7, 0}, { 6, 5}, { 8, 1}, { 5, 4}, { 1, 2}, { 0, 3}, { 4, 4}, { 3, 3}, { 2, 2}, 
	{ 8, 0}, { 1, 3}, { 7, 5}, { 0, 4}, { 4, 4}, { 3, 3}, { 2, 2}, { 5, 5}, { 6, 6}, 
	{ 1, 0}, { 2, 3}, { 8, 7}, { 3, 4}, { 7, 6}, { 0, 5}, { 4, 4}, { 5, 5}, { 6, 6}, 
	{ 7, 0}, { 2, 1}, { 4, 3}, { 0, 6}, { 1, 1}, { 3, 3}, { 5, 5}, { 6, 6}, { 8, 8}, 
	{ 6, 0}, { 1, 2}, { 3, 4}, { 0, 7}, { 2, 2}, { 4, 4}, { 5, 5}, { 7, 7}, { 8, 8}, 
	{ 5, 0}, { 1, 3}, { 2, 4}, { 0, 8}, { 3, 3}, { 4, 4}, { 6, 6}, { 7, 7}, { 8, 8}
};


static int PMVFAST(unsigned char *current, unsigned char *reference, int stride, MV *motion_vectors, int block_width, int block_height, int *SADs, t_SAD SAD)
{
	int area_multiplier = block_width*block_height;
	static const int T=1; // PMVFAST first threshold, per pixel
	int temp_SAD;
	int min_SAD;
	MV median;
	int median_norm;
	int T1;
	int T2;
	int stride_MB = (stride-2*HORIZONTAL_PADDING)/MB_WIDTH+2;

	//predictors are the MV: (0,0), (motion_vector[-1]:left), (motion_vector[-mv_stride]:top), (motion_vector[-mv_stride+1]:top_right),
	//                              (motion_vector[0]:temporally collocated)
	//find mean or median of predictors
	calc_median(&motion_vectors[-1],&motion_vectors[-stride_MB],&motion_vectors[-stride_MB+1],&median);
	//calulate SAD of median
	min_SAD = SAD(current, reference+median.y*stride+median.x, stride, block_width, block_height, 65535);
	if(min_SAD>=T*area_multiplier)
	{
		//calculate SAD of other predictors
		//find minimum of the SAD of predictors left, top, top_right and store it in T1
		//find the best SAD of all the predictors
		// Center (0, 0)
		median_norm=abs(median.x)+abs(median.y);
		temp_SAD = SAD(current, reference, stride, block_width, block_height, min_SAD);
		if(temp_SAD<min_SAD)
		{
			min_SAD = temp_SAD;
			median.y = 0;
			median.x = 0;
		}
		// Left
		temp_SAD = SAD(current, reference+motion_vectors[-1].y*stride+motion_vectors[-1].x, stride, block_width, block_height, min_SAD);
		if(temp_SAD<min_SAD)
		{
			min_SAD = temp_SAD;
			median.y = motion_vectors[-1].y;
			median.x = motion_vectors[-1].x;
		}
		// Top
		temp_SAD = SAD(current, reference+motion_vectors[-stride_MB].y*stride+motion_vectors[-stride_MB].x, stride, block_width, block_height, min_SAD);
		if(temp_SAD<min_SAD)
		{
			min_SAD = temp_SAD;
			median.y = motion_vectors[-stride_MB].y;
			median.x = motion_vectors[-stride_MB].x;
		}
		// Top-right
		temp_SAD = SAD(current, reference+motion_vectors[-stride_MB+1].y*stride+motion_vectors[-stride_MB+1].x, stride, block_width, block_height, min_SAD);
		if(temp_SAD<min_SAD)
		{
			min_SAD = temp_SAD;
			median.y = motion_vectors[-stride_MB+1].y;
			median.x = motion_vectors[-stride_MB+1].x;
		}
		// Temporal prediction
		temp_SAD = SAD(current, reference+motion_vectors[0].y*stride+motion_vectors[0].x, stride, block_width, block_height, min_SAD);
		if(temp_SAD<min_SAD)
		{
			min_SAD = temp_SAD;
			median.y = motion_vectors[0].y;
			median.x = motion_vectors[0].x;
		}
		T1 = SADs[-1];
		if(SADs[-stride_MB]<T1)
		{
			T1 = SADs[-stride_MB];
		}
		if(SADs[-stride_MB+1]<T1)
		{
			T1 = SADs[-stride_MB+1];
		}

		T2 = T1 + area_multiplier;
		if(T1>4*area_multiplier)
		{
			T1 = 4*area_multiplier;
		}
		//else if(T1<2*area_multiplier)
		//{
		//	T1 = 2*area_multiplier;
		//}

		if(min_SAD>=T1)
		{
#if 0
			min_SAD = diamond_search(current, reference+median.y*stride+median.x, stride, &median, block_width, block_height, small_block, 9, min_SAD, next_small_block, SAD);
#else
			//if(T2>7*area_multiplier)
			//	T2 = 7*area_multiplier;
			if(T2>=6*area_multiplier && median_norm==0)
			{
				//large-diamond
				min_SAD = diamond_search(current, reference+median.y*stride+median.x, stride, &median, block_width, block_height, large_diamond, 9, min_SAD, next_large_diamond, SAD);
			}
			//small-diamond search
			min_SAD = diamond_search(current, reference+median.y*stride+median.x, stride, &median, block_width, block_height, small_diamond, 5, min_SAD, next_small_diamond, SAD);
#endif
		}
	}
	motion_vectors[0].y = median.y;
	motion_vectors[0].x = median.x;

	return min_SAD;	
}


static int full_search(unsigned char *current,unsigned char *reference,int stride, MV *motion_vectors,int block_width,int block_height, int *SADs, t_SAD SAD)
{
	int mvx, mvy;
	int temp_SAD, min_SAD;
	static const int search_range = 24;
	int limit;

	// outwards spiral scanning
	//20 21 22 23 24
	//19  6  7  8  9
	//18  5  0  1 10
	//17  4  3  2 11
	//16 15 14 13 12
	//
	min_SAD = 65535;
	min_SAD = SAD(current,reference,stride,block_width,block_height, min_SAD);
	motion_vectors->y = 0;
	motion_vectors->x = 0;
	for(mvy = 0, mvx = 1, limit=1;limit<search_range;limit++)
	{
		for(;mvy<limit;mvy++,reference += stride)
		{
			temp_SAD = SAD(current,reference+mvx,stride,block_width,block_height, min_SAD);
			if(temp_SAD<min_SAD)
			{
				min_SAD = temp_SAD;
				motion_vectors->y = mvy;
				motion_vectors->x = mvx;
			}
		}
		for(;mvx>(-limit);mvx--)
		{
			temp_SAD = SAD(current,reference+mvx,stride,block_width,block_height, min_SAD);
			if(temp_SAD<min_SAD)
			{
				min_SAD = temp_SAD;
				motion_vectors->y = mvy;
				motion_vectors->x = mvx;
			}
		}
		for(;mvy>(-limit);mvy--,reference -= stride)
		{
			temp_SAD = SAD(current,reference+mvx,stride,block_width,block_height, min_SAD);
			if(temp_SAD<min_SAD)
			{
				min_SAD = temp_SAD;
				motion_vectors->y = mvy;
				motion_vectors->x = mvx;
			}
		}
		for(;mvx<=limit;mvx++)
		{
			temp_SAD = SAD(current,reference+mvx,stride,block_width,block_height, min_SAD);
			if(temp_SAD<min_SAD)
			{
				min_SAD = temp_SAD;
				motion_vectors->y = mvy;
				motion_vectors->x = mvx;
			}
		}
	}

	return min_SAD;
}


static void interpolate_mv(MV *mv1, MV *pMV, int width, int height, int block_width, int block_height, int pos_x, int pos_y, short td1)
{
	mv1->y = (td1*pMV->y+16384)>>15;
	mv1->x = (td1*pMV->x+16384)>>15;
	mv1->y = RANGE_CLIP(-block_height-pos_y, mv1->y, height-pos_y);
	mv1->x = RANGE_CLIP(-block_width-pos_x, mv1->x, width-pos_x);
}

static void complementary_mv(MV *mv2, MV *mv1, MV *pMV, int width, int height, int block_width, int block_height, int pos_x, int pos_y)
{
	mv2->y = mv1->y-pMV->y;
	mv2->x = mv1->x-pMV->x;
	mv2->y = RANGE_CLIP(-block_height-pos_y, mv2->y, height-pos_y);
	mv2->x = RANGE_CLIP(-block_width-pos_x, mv2->x, width-pos_x);
}

static void copy_mv(MV *mv2, MV *mv1)
{
	mv2->y = mv1->y;
	mv2->x = mv1->x;
}

int spatial_search(unsigned char *current,unsigned char *reference,
				   int stride,int width,int height,int block_width,int block_height,
				   MV *motion_vectors,int *SADs,int *mses,unsigned char *MB_modes,
				   int *count_I,int *bits)
{
	int i, j;
	int block_mse, line_mse, mse;
	int stride_MB = width/MB_WIDTH+2;
	int mbx;
	int val,k;

	mse = 0;
	(*count_I) = 0;
	(*bits) = 0;
	for(i=0;i<height;i+=block_height)
	{
		// Special case for 1920x1080 - works for all non-multiple of MBs height
		if(i>height-block_height)
		{
			block_height = height-i;
		}
		line_mse = 0;
		for(j=0,mbx=0;j<width;j+=block_width,mbx++)
		{
			int block_mse16, block_mse8;

			// Try 16x16 mode first
			block_mse16 = fast_variance16(current+j,stride,16,block_height);
			// Now 8x8 mode
			if(block_height>8)
			{
				block_mse8  = fast_variance8(current+j,  stride,8,8);
				block_mse8 += fast_variance8(current+8+j,stride,8,8);
				block_mse8 += fast_variance8(current+8*stride+j,  stride,8,block_height-8);
				block_mse8 += fast_variance8(current+8*stride+8+j,stride,8,block_height-8);
			}
			else
			{
				block_mse8  = fast_variance8(current+j,  stride,8,block_height);
				block_mse8 += fast_variance8(current+8+j,stride,8,block_height);
			}
			if(block_mse8<NORMALIZE(block_mse16))
			{
				block_mse = block_mse8;
			}
			else
			{
				block_mse = block_mse16;
			}
			mses[mbx] = block_mse;
			line_mse += block_mse;
			MB_modes[mbx] = 0;
			for(val=1,k=0;val<=block_mse;k++,val<<=1)
			{
			}
			(*bits)+=k;
		}
		(*count_I)+=mbx;
		mse       += (line_mse+128)>>8;
		current   += block_height*stride;
		mses      += stride_MB;
		MB_modes  += stride_MB;
	}

	return mse;
}


int motion_search(unsigned char *current,unsigned char *reference,
				  int stride,int width,int height,int block_width,int block_height,
				  MV *motion_vectors,int *SADs,int *mses,unsigned char *MB_modes,
				  int *count_I,int *count_P,int *bits)
{
	int i, j;
	int temp_SAD;
	int block_mse, line_mse, mse;
	int stride_MB = width/MB_WIDTH+2;
	int mbx;
	int val,k;

	mse = 0;
	*count_I = *count_P = 0;
	*bits = 0;
	for(i=0;i<height;i+=block_height)
	{
		// Special case for 1920x1080 - works for all non-multiple of MBs height
		if(i>height-block_height)
		{
			block_height = height-i;
		}
		line_mse = 0;
		for(j=0,mbx=0;j<width;j+=block_width,mbx++)
		{
			int var;
			int block_mse16, block_mse8;
			MV backup_MV;
			int backup_SAD;

			// Try 16x16 mode first
			block_mse16 = fast_variance16(current+j,stride,16,block_height);
			// Now 8x8 mode
			if(block_height>8)
			{
				block_mse8  = fast_variance8(current+j,  stride,8,8);
				block_mse8 += fast_variance8(current+8+j,stride,8,8);
				block_mse8 += fast_variance8(current+8*stride+j,  stride,8,block_height-8);
				block_mse8 += fast_variance8(current+8*stride+8+j,stride,8,block_height-8);
			}
			else
			{
				block_mse8  = fast_variance8(current+j,  stride,8,block_height);
				block_mse8 += fast_variance8(current+8+j,stride,8,block_height);
			}
			if(block_mse8<NORMALIZE(block_mse16))
			{
				var = block_mse8;
			}
			else
			{
				var = block_mse16;
			}
			// Try 16x16 mode first
			temp_SAD = SEARCH_MV(current+j,reference+j,stride,&motion_vectors[mbx],16,block_height,&SADs[mbx],fastSAD16);
			block_mse16 = fast_calc_mse16(current+j,reference+j+motion_vectors[mbx].y*stride+motion_vectors[mbx].x,stride,16,block_height);
			copy_mv(&backup_MV,&motion_vectors[mbx]);
			backup_SAD  = temp_SAD;
			// Now 8x8 mode
			if(block_height>8)
			{
				temp_SAD = SEARCH_MV(current+j,reference+j,stride,&motion_vectors[mbx],8,8,&SADs[mbx],fastSAD8);
				block_mse8 = fast_calc_mse8(current+j,reference+j+motion_vectors[mbx].y*stride+motion_vectors[mbx].x,stride,8,8);
				copy_mv(&motion_vectors[mbx],&backup_MV);
				temp_SAD = SEARCH_MV(current+8+j,reference+8+j,stride,&motion_vectors[mbx],8,8,&SADs[mbx],fastSAD8);
				block_mse8 += fast_calc_mse8(current+8+j,reference+8+j+motion_vectors[mbx].y*stride+motion_vectors[mbx].x,stride,8,8);
				copy_mv(&motion_vectors[mbx],&backup_MV);
				temp_SAD = SEARCH_MV(current+8*stride+j,reference+8*stride+j,stride,&motion_vectors[mbx],8,block_height-8,&SADs[mbx],fastSAD8);
				block_mse8 += fast_calc_mse8(current+8*stride+j,reference+8*stride+j+motion_vectors[mbx].y*stride+motion_vectors[mbx].x,stride,8,block_height-8);
				copy_mv(&motion_vectors[mbx],&backup_MV);
				temp_SAD = SEARCH_MV(current+8*stride+8+j,reference+8*stride+8+j,stride,&motion_vectors[mbx],8,block_height-8,&SADs[mbx],fastSAD8);
				block_mse8 += fast_calc_mse8(current+8*stride+8+j,reference+8*stride+8+j+motion_vectors[mbx].y*stride+motion_vectors[mbx].x,stride,8,block_height-8);
				copy_mv(&motion_vectors[mbx],&backup_MV);
				temp_SAD = backup_SAD;
			}
			else
			{
				temp_SAD = SEARCH_MV(current+j,reference+j,stride,&motion_vectors[mbx],8,block_height,&SADs[mbx],fastSAD8);
				block_mse8 = fast_calc_mse8(current+j,reference+j+motion_vectors[mbx].y*stride+motion_vectors[mbx].x,stride,8,block_height);
				copy_mv(&motion_vectors[mbx],&backup_MV);
				temp_SAD = SEARCH_MV(current+8+j,reference+8+j,stride,&motion_vectors[mbx],8,block_height,&SADs[mbx],fastSAD8);
				block_mse8 += fast_calc_mse8(current+8+j,reference+8+j+motion_vectors[mbx].y*stride+motion_vectors[mbx].x,stride,8,block_height);
				copy_mv(&motion_vectors[mbx],&backup_MV);
				temp_SAD = backup_SAD;
			}
			if(block_mse8<NORMALIZE(block_mse16))
			{
				block_mse = block_mse8;
			}
			else
			{
				block_mse = block_mse16;
			}

			if(block_mse<var)
			{
				SADs[mbx] = temp_SAD;
				MB_modes[mbx] = 1;
				(*count_P)++;
			}
			else
			{
				block_mse = var;
				motion_vectors[mbx].y = motion_vectors[mbx].x = 0;
				//SADs[mbx] = var;
				SADs[mbx] = 0;
				MB_modes[mbx] = 0;
				(*count_I)++;
			}
			mses[mbx] = block_mse;
			line_mse += block_mse;
			for(val=1,k=0;val<=block_mse;k++,val<<=1)
			{
			}
			(*bits)+=k;
		}
		mse       += (line_mse+128)>>8;
		current   += block_height*stride;
		reference += block_height*stride;
		SADs      += stride_MB;
		motion_vectors += stride_MB;
		mses      += stride_MB;
		MB_modes  += stride_MB;
	}

	return mse;
}


//Assume td1+td2 = 32768 = 2^15
int bidir_motion_search(unsigned char *current,unsigned char *reference1,unsigned char *reference2,
						int stride,int width,int height,int block_width,int block_height,
						MV *P_motion_vectors,MV *motion_vectors1,MV *motion_vectors2,
						int *SADs1,int *SADs2,int *mses,unsigned char *MB_modes,short td1,short td2,
						int *count_I,int *count_P,int *count_B,int *bits)
{
	int i, j;
	int block_mse, block_mse1, block_mse2, line_mse, mse;
	int stride_MB = width/MB_WIDTH+2;
	int mbx;
	MV td = { td1, td2};
	int temp_SAD;
	int val, k;

	mse = 0;
	*count_I = *count_P = *count_B = 0;
	*bits = 0;
	for(i=0;i<height;i+=block_height)
	{
		// Special case for 1920x1080 - works for all non-multiple of MBs height
		if(i>height-block_height)
		{
			block_height = height-i;
		}
		line_mse = 0;
		for(j=0,mbx=0;j<width;j+=block_width,mbx++)
		{
			int var;
			MV *mv1 = &motion_vectors1[mbx];
			MV *mv2 = &motion_vectors2[mbx];
			int block_mse16, block_mse8;
			MV backup_MV;
			MV tempMV1[4];
			MV tempMV2[4];
			int tempMSEs[4];
			int backup_SAD;

			// Try 16x16 mode first
			block_mse16 = fast_variance16(current+j,stride,16,block_height);
			// Now 8x8 mode
			if(block_height>8)
			{
				block_mse8  = fast_variance8(current+j,  stride,8,8);
				block_mse8 += fast_variance8(current+8+j,stride,8,8);
				block_mse8 += fast_variance8(current+8*stride+j,  stride,8,block_height-8);
				block_mse8 += fast_variance8(current+8*stride+8+j,stride,8,block_height-8);
			}
			else
			{
				block_mse8  = fast_variance8(current+j,  stride,8,block_height);
				block_mse8 += fast_variance8(current+8+j,stride,8,block_height);
			}
			if(block_mse8<NORMALIZE(block_mse16))
			{
				var = block_mse8;
			}
			else
			{
				var = block_mse16;
			}

			if(td1<=td2)
			{
				interpolate_mv(mv1, &P_motion_vectors[mbx], width, height, block_width, block_height, j, i, td1);

				// Try 16x16 mode first
				temp_SAD = SEARCH_MV(current+j,reference1+j,stride,mv1,16,block_height,&SADs1[mbx],fastSAD16);
				block_mse16 = fast_calc_mse16(current+j,reference1+j+mv1->y*stride+mv1->x,stride,16,block_height);
				copy_mv(&backup_MV, mv1);
				backup_SAD  = temp_SAD;
				// Now 8x8 mode
				if(block_height>8)
				{
					temp_SAD = SEARCH_MV(current+j,reference1+j,stride,mv1,8,8,&SADs1[mbx],fastSAD8);
					tempMSEs[0] = fast_calc_mse8(current+j,reference1+j+mv1->y*stride+mv1->x,stride,8,8);
					copy_mv(&tempMV1[0], mv1);
					copy_mv(mv1, &backup_MV);
					temp_SAD = SEARCH_MV(current+8+j,reference1+8+j,stride,mv1,8,8,&SADs1[mbx],fastSAD8);
					tempMSEs[1] = fast_calc_mse8(current+8+j,reference1+8+j+mv1->y*stride+mv1->x,stride,8,8);
					copy_mv(&tempMV1[1], mv1);
					copy_mv(mv1, &backup_MV);
					temp_SAD = SEARCH_MV(current+8*stride+j,reference1+8*stride+j,stride,mv1,8,block_height-8,&SADs1[mbx],fastSAD8);
					tempMSEs[2] = fast_calc_mse8(current+8*stride+j,reference1+8*stride+j+mv1->y*stride+mv1->x,stride,8,block_height-8);
					copy_mv(&tempMV1[2], mv1);
					copy_mv(mv1, &backup_MV);
					temp_SAD = SEARCH_MV(current+8*stride+8+j,reference1+8*stride+8+j,stride,mv1,8,block_height-8,&SADs1[mbx],fastSAD8);
					tempMSEs[3] = fast_calc_mse8(current+8*stride+8+j,reference1+8*stride+8+j+mv1->y*stride+mv1->x,stride,8,block_height-8);
					copy_mv(&tempMV1[3], mv1);
					copy_mv(mv1, &backup_MV);
					temp_SAD = backup_SAD;
				}
				else
				{
					temp_SAD = SEARCH_MV(current+j,reference1+j,stride,mv1,8,block_height,&SADs1[mbx],fastSAD8);
					tempMSEs[0] = fast_calc_mse8(current+j,reference1+j+mv1->y*stride+mv1->x,stride,8,block_height);
					copy_mv(&tempMV1[0], mv1);
					copy_mv(mv1, &backup_MV);
					temp_SAD = SEARCH_MV(current+8+j,reference1+8+j,stride,mv1,8,block_height,&SADs1[mbx],fastSAD8);
					tempMSEs[1] = fast_calc_mse8(current+8+j,reference1+8+j+mv1->y*stride+mv1->x,stride,8,block_height);
					copy_mv(&tempMV1[1], mv1);
					copy_mv(mv1, &backup_MV);
					temp_SAD = backup_SAD;
					tempMSEs[2] = tempMSEs[3] = 0;
				}
				block_mse1 = block_mse16;

				if(block_mse1<var)
				{
					SADs1[mbx] = temp_SAD;
				}
				else
				{
					SADs1[mbx] = 0;
				}

				complementary_mv(mv2, mv1, &P_motion_vectors[mbx], width, height, block_width, block_height, j, i);

				// Try 16x16 mode first
				temp_SAD = SEARCH_MV(current+j,reference2+j,stride,mv2,16,block_height,&SADs2[mbx],fastSAD16);
				block_mse16 = fast_calc_mse16(current+j,reference2+j+mv2->y*stride+mv2->x,stride,16,block_height);
				copy_mv(&backup_MV, mv2);
				backup_SAD  = temp_SAD;
				// Now 8x8 mode
				if(block_height>8)
				{
					temp_SAD = SEARCH_MV(current+j,reference2+j,stride,mv2,8,8,&SADs2[mbx],fastSAD8);
					block_mse8 = fast_calc_mse8(current+j,reference2+j+mv2->y*stride+mv2->x,stride,8,8);
					if(block_mse8<tempMSEs[0])
					{
						tempMSEs[0] = block_mse8;
					}
					copy_mv(&tempMV2[0], mv2);
					copy_mv(mv2, &backup_MV);
					temp_SAD = SEARCH_MV(current+8+j,reference2+8+j,stride,mv2,8,8,&SADs2[mbx],fastSAD8);
					block_mse8 = fast_calc_mse8(current+8+j,reference2+8+j+mv2->y*stride+mv2->x,stride,8,8);
					if(block_mse8<tempMSEs[1])
					{
						tempMSEs[1] = block_mse8;
					}
					copy_mv(&tempMV2[1], mv2);
					copy_mv(mv2, &backup_MV);
					temp_SAD = SEARCH_MV(current+8*stride+j,reference2+8*stride+j,stride,mv2,8,block_height-8,&SADs2[mbx],fastSAD8);
					block_mse8 = fast_calc_mse8(current+8*stride+j,reference2+8*stride+j+mv2->y*stride+mv2->x,stride,8,block_height-8);
					if(block_mse8<tempMSEs[2])
					{
						tempMSEs[2] = block_mse8;
					}
					copy_mv(&tempMV2[2], mv2);
					copy_mv(mv2, &backup_MV);
					temp_SAD = SEARCH_MV(current+8*stride+8+j,reference2+8*stride+8+j,stride,mv2,8,block_height-8,&SADs2[mbx],fastSAD8);
					block_mse8 = fast_calc_mse8(current+8*stride+8+j,reference2+8*stride+8+j+mv2->y*stride+mv2->x,stride,8,block_height-8);
					if(block_mse8<tempMSEs[3])
					{
						tempMSEs[3] = block_mse8;
					}
					copy_mv(&tempMV2[3], mv2);
					copy_mv(mv2, &backup_MV);
					temp_SAD = backup_SAD;
				}
				else
				{
					temp_SAD = SEARCH_MV(current+j,reference2+j,stride,mv2,8,block_height,&SADs2[mbx],fastSAD8);
					block_mse8 = fast_calc_mse8(current+j,reference2+j+mv2->y*stride+mv2->x,stride,8,block_height);
					if(block_mse8<tempMSEs[0])
					{
						tempMSEs[0] = block_mse8;
					}
					copy_mv(&tempMV2[0], mv2);
					copy_mv(mv2, &backup_MV);
					temp_SAD = SEARCH_MV(current+8+j,reference2+8+j,stride,mv2,8,block_height,&SADs2[mbx],fastSAD8);
					block_mse8 = fast_calc_mse8(current+8+j,reference2+8+j+mv2->y*stride+mv2->x,stride,8,block_height);
					if(block_mse8<tempMSEs[1])
					{
						tempMSEs[1] = block_mse8;
					}
					copy_mv(&tempMV2[1], mv2);
					copy_mv(mv2, &backup_MV);
					temp_SAD = backup_SAD;
				}
				block_mse2 = block_mse16;

				if(block_mse2<var)
				{
					SADs2[mbx] = temp_SAD;
				}
				else
				{
					SADs2[mbx] = 0;
				}
			}
			else
			{
				interpolate_mv(mv2, &P_motion_vectors[mbx], width, height, block_width, block_height, j, i, -td2);

				// Try 16x16 mode first
				temp_SAD = SEARCH_MV(current+j,reference2+j,stride,mv2,16,block_height,&SADs2[mbx],fastSAD16);
				block_mse16 = fast_calc_mse16(current+j,reference2+j+mv2->y*stride+mv2->x,stride,16,block_height);
				copy_mv(&backup_MV, mv2);
				backup_SAD  = temp_SAD;
				// Now 8x8 mode
				if(block_height>8)
				{
					temp_SAD = SEARCH_MV(current+j,reference2+j,stride,mv2,8,8,&SADs2[mbx],fastSAD8);
					tempMSEs[0] = fast_calc_mse8(current+j,reference2+j+mv2->y*stride+mv2->x,stride,8,8);
					copy_mv(&tempMV2[0], mv2);
					copy_mv(mv2, &backup_MV);
					temp_SAD = SEARCH_MV(current+8+j,reference2+8+j,stride,mv2,8,8,&SADs2[mbx],fastSAD8);
					tempMSEs[1] = fast_calc_mse8(current+8+j,reference2+8+j+mv2->y*stride+mv2->x,stride,8,8);
					copy_mv(&tempMV2[1], mv2);
					copy_mv(mv2, &backup_MV);
					temp_SAD = SEARCH_MV(current+8*stride+j,reference2+8*stride+j,stride,mv2,8,block_height-8,&SADs2[mbx],fastSAD8);
					tempMSEs[2] = fast_calc_mse8(current+8*stride+j,reference2+8*stride+j+mv2->y*stride+mv2->x,stride,8,block_height-8);
					copy_mv(&tempMV2[2], mv2);
					copy_mv(mv2, &backup_MV);
					temp_SAD = SEARCH_MV(current+8*stride+8+j,reference2+8*stride+8+j,stride,mv2,8,block_height-8,&SADs2[mbx],fastSAD8);
					tempMSEs[3] = fast_calc_mse8(current+8*stride+8+j,reference2+8*stride+8+j+mv2->y*stride+mv2->x,stride,8,block_height-8);
					copy_mv(&tempMV2[3], mv2);
					copy_mv(mv2, &backup_MV);
					temp_SAD = backup_SAD;
				}
				else
				{
					temp_SAD = SEARCH_MV(current+j,reference2+j,stride,mv2,8,block_height,&SADs2[mbx],fastSAD8);
					tempMSEs[0] = fast_calc_mse8(current+j,reference2+j+mv2->y*stride+mv2->x,stride,8,block_height);
					copy_mv(&tempMV2[0], mv2);
					copy_mv(mv2, &backup_MV);
					temp_SAD = SEARCH_MV(current+8+j,reference2+8+j,stride,mv2,8,block_height,&SADs2[mbx],fastSAD8);
					tempMSEs[1] = fast_calc_mse8(current+8+j,reference2+8+j+mv2->y*stride+mv2->x,stride,8,block_height);
					copy_mv(&tempMV2[1], mv2);
					copy_mv(mv2, &backup_MV);
					temp_SAD = backup_SAD;
					tempMSEs[2] = tempMSEs[3] = 0;
				}
				block_mse2 = block_mse16;

				if(block_mse2<var)
				{
					SADs2[mbx] = temp_SAD;
				}
				else
				{
					SADs2[mbx] = 0;
				}

				complementary_mv(mv1, &P_motion_vectors[mbx], mv2, width, height, block_width, block_height, j, i);

				// Try 16x16 mode first
				temp_SAD = SEARCH_MV(current+j,reference1+j,stride,mv1,16,block_height,&SADs1[mbx],fastSAD16);
				block_mse16 = fast_calc_mse16(current+j,reference1+j+mv1->y*stride+mv1->x,stride,16,block_height);
				copy_mv(&backup_MV, mv1);
				backup_SAD  = temp_SAD;
				// Now 8x8 mode
				if(block_height>8)
				{
					temp_SAD = SEARCH_MV(current+j,reference1+j,stride,mv1,8,8,&SADs1[mbx],fastSAD8);
					block_mse8 = fast_calc_mse8(current+j,reference1+j+mv1->y*stride+mv1->x,stride,8,8);
					if(block_mse8<tempMSEs[0])
					{
						tempMSEs[0] = block_mse8;
					}
					copy_mv(&tempMV1[0], mv1);
					copy_mv(mv1, &backup_MV);
					temp_SAD = SEARCH_MV(current+8+j,reference1+8+j,stride,mv1,8,8,&SADs1[mbx],fastSAD8);
					block_mse8 = fast_calc_mse8(current+8+j,reference1+8+j+mv1->y*stride+mv1->x,stride,8,8);
					if(block_mse8<tempMSEs[1])
					{
						tempMSEs[1] = block_mse8;
					}
					copy_mv(&tempMV1[1], mv1);
					copy_mv(mv1, &backup_MV);
					temp_SAD = SEARCH_MV(current+8*stride+j,reference1+8*stride+j,stride,mv1,8,block_height-8,&SADs1[mbx],fastSAD8);
					block_mse8 = fast_calc_mse8(current+8*stride+j,reference1+8*stride+j+mv1->y*stride+mv1->x,stride,8,block_height-8);
					if(block_mse8<tempMSEs[2])
					{
						tempMSEs[2] = block_mse8;
					}
					copy_mv(&tempMV1[2], mv1);
					copy_mv(mv1, &backup_MV);
					temp_SAD = SEARCH_MV(current+8*stride+8+j,reference1+8*stride+8+j,stride,mv1,8,block_height-8,&SADs1[mbx],fastSAD8);
					block_mse8 = fast_calc_mse8(current+8*stride+8+j,reference1+8*stride+8+j+mv1->y*stride+mv1->x,stride,8,block_height-8);
					if(block_mse8<tempMSEs[3])
					{
						tempMSEs[3] = block_mse8;
					}
					copy_mv(&tempMV1[3], mv1);
					copy_mv(mv1, &backup_MV);
					temp_SAD = backup_SAD;
				}
				else
				{
					temp_SAD = SEARCH_MV(current+j,reference1+j,stride,mv1,8,block_height,&SADs1[mbx],fastSAD8);
					block_mse8 = fast_calc_mse8(current+j,reference1+j+mv1->y*stride+mv1->x,stride,8,block_height);
					if(block_mse8<tempMSEs[0])
					{
						tempMSEs[0] = block_mse8;
					}
					copy_mv(&tempMV1[0], mv1);
					copy_mv(mv1, &backup_MV);
					temp_SAD = SEARCH_MV(current+8+j,reference1+8+j,stride,mv1,8,block_height,&SADs1[mbx],fastSAD8);
					block_mse8 = fast_calc_mse8(current+8+j,reference1+8+j+mv1->y*stride+mv1->x,stride,8,block_height);
					if(block_mse8<tempMSEs[1])
					{
						tempMSEs[1] = block_mse8;
					}
					copy_mv(&tempMV1[1], mv1);
					copy_mv(mv1, &backup_MV);
					temp_SAD = backup_SAD;
				}
				block_mse1 = block_mse16;

				if(block_mse1<var)
				{
					SADs1[mbx] = temp_SAD;
				}
				else
				{
					SADs1[mbx] = 0;
				}
			}
			// Try 16x16 mode first
			block_mse16 = fast_bidir_mse16(&current[j], &reference1[j+mv1->y*stride+mv1->x], &reference2[j+mv2->y*stride+mv2->x], 
							  stride, 16, block_height, &td);
			// Now 8x8 mode
			if(block_height>8)
			{
				block_mse8 = fast_bidir_mse8(&current[j], &reference1[j+tempMV1[0].y*stride+tempMV1[0].x], &reference2[j+tempMV2[0].y*stride+tempMV2[0].x], 
								  stride, 8, 8, &td);
				if(block_mse8<tempMSEs[0])
				{
					tempMSEs[0] = block_mse8;
				}
				block_mse8 = fast_bidir_mse8(&current[j+8], &reference1[j+tempMV1[1].y*stride+tempMV1[1].x+8], &reference2[j+tempMV2[1].y*stride+tempMV2[1].x+8], 
								  stride, 8, 8, &td);
				if(block_mse8<tempMSEs[1])
				{
					tempMSEs[1] = block_mse8;
				}
				block_mse8 = fast_bidir_mse8(&current[j+8*stride], &reference1[j+(8+tempMV1[2].y)*stride+tempMV1[2].x], &reference2[j+(8+tempMV2[2].y)*stride+tempMV2[2].x], 
								  stride, 8, block_height-8, &td);
				if(block_mse8<tempMSEs[2])
				{
					tempMSEs[2] = block_mse8;
				}
				block_mse8 = fast_bidir_mse8(&current[j+8*stride+8], &reference1[j+(8+tempMV1[3].y)*stride+tempMV1[3].x+8], &reference2[j+(8+tempMV2[3].y)*stride+tempMV2[3].x+8], 
								  stride, 8, block_height-8, &td);
				if(block_mse8<tempMSEs[3])
				{
					tempMSEs[3] = block_mse8;
				}
			}
			else
			{
				block_mse8 = fast_bidir_mse8(&current[j], &reference1[j+tempMV1[0].y*stride+tempMV1[0].x], &reference2[j+tempMV2[0].y*stride+tempMV2[0].x], 
								  stride, 8, block_height, &td);
				if(block_mse8<tempMSEs[0])
				{
					tempMSEs[0] = block_mse8;
				}
				block_mse8 = fast_bidir_mse8(&current[j+8], &reference1[j+tempMV1[1].y*stride+tempMV1[1].x+8], &reference2[j+tempMV2[1].y*stride+tempMV2[1].x+8], 
								  stride, 8, block_height, &td);
				if(block_mse8<tempMSEs[1])
				{
					tempMSEs[1] = block_mse8;
				}
			}
			block_mse8 = tempMSEs[0]+tempMSEs[1]+tempMSEs[2]+tempMSEs[3];

			if(block_mse16<block_mse1 && block_mse16<block_mse2)
			{
				MB_modes[mbx] = 3;
			}
			else if(block_mse2<block_mse1)
			{
				block_mse16 = block_mse2;
				MB_modes[mbx] = 2;
			}
			else
			{
				block_mse16 = block_mse1;
				MB_modes[mbx] = 1;
			}

			if(block_mse8<NORMALIZE(block_mse16))
			{
				block_mse = block_mse8;
				MB_modes[mbx] = 4;
			}
			else
			{
				block_mse = block_mse16;
			}

			if(block_mse<var)
			{
			}
			else
			{
				block_mse = var;
				MB_modes[mbx] = 0;
			}

			switch (MB_modes[mbx])
			{
			case 0: (*count_I)++; break;
			case 1: 
			case 2: (*count_P)++; break;
			case 3:
			case 4: (*count_B)++; break;
			default: break;
			}

			mses[mbx] = block_mse;
			line_mse += block_mse;
			for(val=1,k=0;val<=block_mse;k++,val<<=1)
			{
			}
			(*bits)+=k;
		}
		mse       += (line_mse+128)>>8;
		current   += block_height*stride;
		reference1 += block_height*stride;
		reference2 += block_height*stride;
		P_motion_vectors += stride_MB;
		motion_vectors1 += stride_MB;
		motion_vectors2 += stride_MB;
		SADs1     += stride_MB;
		SADs2     += stride_MB;
		mses      += stride_MB;
		MB_modes  += stride_MB;
	}

	return mse;
}
