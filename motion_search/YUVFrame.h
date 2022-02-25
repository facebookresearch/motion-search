#pragma once

#include <motion_search/inc/common.h>
#include <motion_search/inc/memory.h>
#include "IVideoSequenceReader.h"

class YUVFrame
{
public:
	virtual ~YUVFrame(void);
	YUVFrame(IVideoSequenceReader *rdr);

	inline uint8_t *y(void) { return m_pY; }
	inline uint8_t *u(void) { return m_pU; }
	inline uint8_t *v(void) { return m_pV; }
	inline int pos(void) { return m_pos; }
	inline int width(void) { return m_width; }
	inline int height(void) { return m_height; }
	inline int stride(void) { return m_stride; }

	void swapFrame (YUVFrame *other);
	void readNextFrame(void);
	void boundaryExtend(void);

private:
	const int m_width;
	const int m_height;
	const int m_stride;
	const int m_padded_height;

    memory::aligned_unique_ptr<uint8_t> m_pFrame;
	uint8_t *m_pY;
	uint8_t *m_pU;
	uint8_t *m_pV;

	IVideoSequenceReader *m_pReader;

	int m_pos;

    YUVFrame(YUVFrame &) = delete;
    YUVFrame & operator = (YUVFrame &) = delete;
};

