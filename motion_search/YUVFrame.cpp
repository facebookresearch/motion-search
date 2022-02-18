#include "YUVFrame.h"

#include <cstdlib>
#include <cstdio>
#include <algorithm>


YUVFrame::YUVFrame(IVideoSequenceReader *rdr) : m_width(rdr->width()),
												m_height(rdr->height()),
												m_stride(rdr->width() + 2 * HORIZONTAL_PADDING),
												m_padded_height(rdr->height() + 2 * VERTICAL_PADDING),
												m_pReader(rdr),
												m_pos(-1)
{
	int frame_size = m_stride * m_padded_height * 3 * sizeof(uint8_t) / 2;

	m_pFrame = (uint8_t *) _aligned_malloc(frame_size, 16);
	if (m_pFrame == NULL) {
		printf("Not enough memory (%d bytes) for YUVFrame\n", frame_size);
		exit(-1);
	}

	int luma_offset = VERTICAL_PADDING * m_stride + HORIZONTAL_PADDING;
	int cr_offset = m_stride * m_padded_height + VER_PADDING_UV * m_stride / 2 + HOR_PADDING_UV;
	int cb_offset = cr_offset + (m_stride/2) * (m_padded_height/2);

	m_pY = m_pFrame + luma_offset;
	m_pU = m_pFrame + cr_offset;
	m_pV = m_pFrame + cb_offset;
}


YUVFrame::~YUVFrame(void)
{
	_aligned_free((void *) m_pFrame);
}


void YUVFrame::swapFrame (YUVFrame *other)
{
	std::swap(this->m_pFrame, other->m_pFrame);
	std::swap(this->m_pY, other->m_pY);
	std::swap(this->m_pU, other->m_pU);
	std::swap(this->m_pV, other->m_pV);
	std::swap(this->m_pos, other->m_pos);
}


void YUVFrame::readNextFrame(void)
{
	m_pos = m_pReader->count();
	m_pReader->read(y(), u(), v());
}


// Pad frame
static void extend_frame(unsigned char *frame_ptr, int stride, int width, int height, int pad_size_x, int pad_size_y)
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
}


void YUVFrame::boundaryExtend(void)
{
	extend_frame(y(), m_stride, m_width, m_height, HORIZONTAL_PADDING, VERTICAL_PADDING);
	extend_frame(u(), m_stride>>1, m_width>>1, m_height>>1, HOR_PADDING_UV, VER_PADDING_UV);
	extend_frame(v(), m_stride>>1, m_width>>1, m_height>>1, HOR_PADDING_UV, VER_PADDING_UV);
}
