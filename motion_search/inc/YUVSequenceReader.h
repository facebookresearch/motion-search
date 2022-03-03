#pragma once

#include <motion_search/inc/common.h>

#include <motion_search/inc/BaseVideoSequenceReader.h>

#include <cstdio>
#include <memory>
#include <string>

class YUVSequenceReader : public BaseVideoSequenceReader
{
public:
    YUVSequenceReader(void) = default;
    ~YUVSequenceReader(void) = default;

    bool Open(unique_file_t file, const std::string& path, const DIM dim);

    bool eof(void) override;
    int nframes(void) override;
    const DIM dim(void) override { return m_dim; }
    ptrdiff_t stride(void) override { return m_stride; }

    bool isOpen(void) override {
        if ((!m_dim.width) || (!m_dim.height) || (!m_file.get())) {
            return false;
        } else {
            return true;
        }
    }

protected:
    void readPicture(uint8_t *pY, uint8_t *pU, uint8_t *pV) override;

    FILE* file(void) {return m_file.get();}

private:
    DIM m_dim = {0, 0};
    ptrdiff_t m_stride = 0;

    std::string m_filename;
    unique_file_t m_file;

    void readComponent(uint8_t *pData, bool isLuma);

    YUVSequenceReader(YUVSequenceReader &) = delete;
    YUVSequenceReader & operator = (YUVSequenceReader &) = delete;
};
