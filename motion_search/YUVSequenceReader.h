#pragma once

#include <motion_search/inc/common.h>
#include "BaseVideoSequenceReader.h"

#include <cstdio>
#include <memory>
#include <string>

class YUVSequenceReader : public BaseVideoSequenceReader
{
public:
    YUVSequenceReader (std::string fname, int width, int height);
    ~YUVSequenceReader (void) = default;
    bool eof(void) override;
    int nframes(void) override;
    int width(void) override { return m_width; }
    int height(void) override { return m_height; }
    ptrdiff_t stride(void) override { return m_stride; }

    bool isOpen(void) override {
        if ((!m_width) || (!m_height) || (!m_file.get())) {
            return false;
        } else {
            return true;
        }
    }

protected:
    void readPicture(uint8_t *pY, uint8_t *pU, uint8_t *pV) override;

private:
    const int m_width = 0;
    const int m_height = 0;
    const ptrdiff_t m_stride = 0;

    const std::string m_filename;
    std::unique_ptr<FILE, file_closer> m_file;

    void readComponent(uint8_t *pData, bool isLuma);

    YUVSequenceReader(YUVSequenceReader &) = delete;
    YUVSequenceReader & operator = (YUVSequenceReader &) = delete;
};
