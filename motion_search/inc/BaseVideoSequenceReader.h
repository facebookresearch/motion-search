
#pragma once

#include <motion_search/inc/common.h>
#include <motion_search/inc/IVideoSequenceReader.h>

class BaseVideoSequenceReader : public IVideoSequenceReader
{
public:
    BaseVideoSequenceReader(void) = default;
    virtual ~BaseVideoSequenceReader(void) = default;

    void read(uint8_t *pY, uint8_t *pU, uint8_t *pV) override;

    int count() override { return m_count; }

protected:
    virtual void readPicture(uint8_t *pY, uint8_t *pU, uint8_t *pV) = 0;

private:
    int m_count = 0;
};
