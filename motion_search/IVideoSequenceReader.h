#pragma once

#include <cstdint>

class IVideoSequenceReader {
public:
	virtual ~IVideoSequenceReader() {};
	virtual void read(uint8_t *pY, uint8_t *pU, uint8_t *pV) = 0;
	virtual bool eof(void) = 0;
	virtual int nframes(void) = 0;
	virtual int count(void) = 0;
	virtual int width(void) = 0;
	virtual int height(void) = 0;
	virtual int stride(void) = 0;
};
