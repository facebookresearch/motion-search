#pragma once

#include <motion_search/inc/common.h>
#include <motion_search/inc/IVideoSequenceReader.h>
#include <motion_search/inc/memory.h>

class YUVFrame
{
public:
    YUVFrame(IVideoSequenceReader *rdr);
    virtual ~YUVFrame(void) = default;

	inline uint8_t *y(void) { return m_pY; }
	inline uint8_t *u(void) { return m_pU; }
	inline uint8_t *v(void) { return m_pV; }
	inline int pos(void) { return m_pos; }
	inline const DIM dim(void) { return m_dim; }
	inline int stride(void) { return m_stride; }

	void swapFrame (YUVFrame *other);
	void readNextFrame(void);
	void boundaryExtend(void);

private:
    const DIM m_dim;
    const int m_stride = 0;
    const int m_padded_height = 0;

    memory::aligned_unique_ptr<uint8_t> m_pFrame;
	uint8_t *m_pY;
	uint8_t *m_pU;
	uint8_t *m_pV;

	IVideoSequenceReader *m_pReader;

	int m_pos;

    YUVFrame(YUVFrame &) = delete;
    YUVFrame & operator = (YUVFrame &) = delete;
};

