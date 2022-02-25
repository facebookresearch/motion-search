#include "frame.h"

#include <memory.h>

// Pad frame
int extend_frame(unsigned char *frame_ptr, int stride, int width, int height, int pad_size_x, int pad_size_y)
{
	int i, j;

	for(i=0;i<height;i++)
	{
		memset(frame_ptr-pad_size_x,frame_ptr[0],pad_size_x);
		memset(frame_ptr+width,frame_ptr[width-1],pad_size_x);
		if(i==0)
		{
			for(j=-pad_size_y;j<0;j++)
			{
				memcpy(frame_ptr+j*stride-pad_size_x,frame_ptr-pad_size_x,stride);
			}
		}
		else if(i==(height-1))
		{
			for(j=1;j<=pad_size_y;j++)
			{
				memcpy(frame_ptr+j*stride-pad_size_x,frame_ptr-pad_size_x,stride);
			}
		}
		frame_ptr += stride;
	}

	return 1;
}

int boundary_extend(unsigned char *ptr1_Y, unsigned char *ptr1_U, unsigned char *ptr1_V, int stride, int width, int height)
{
	extend_frame(ptr1_Y,stride,width,height,HORIZONTAL_PADDING,VERTICAL_PADDING);
	extend_frame(ptr1_U,stride>>1,width>>1,height>>1,HOR_PADDING_UV,VER_PADDING_UV);
	extend_frame(ptr1_V,stride>>1,width>>1,height>>1,HOR_PADDING_UV,VER_PADDING_UV);

	return 1;
}

