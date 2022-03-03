#pragma once

#include <motion_search/inc/common.h>

class IVideoSequenceReader {
public:
    virtual ~IVideoSequenceReader() {};
    virtual void read(uint8_t *pY, uint8_t *pU, uint8_t *pV) = 0;
    virtual bool eof(void) = 0;
    virtual int nframes(void) = 0;
    virtual int count(void) = 0;
    virtual const DIM dim(void) = 0;
    virtual ptrdiff_t stride(void) = 0;
    virtual bool isOpen(void) = 0;
};
