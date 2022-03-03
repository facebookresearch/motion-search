
#include <motion_search/inc/YUVSequenceReader.h>
#include "EOFException.h"

#include <sys/stat.h>

bool YUVSequenceReader::Open(unique_file_t file, const std::string& path,
    const DIM dim) {

    m_dim = dim;
    m_stride = dim.width + 2 * HORIZONTAL_PADDING;
    m_filename = path;
    m_file = std::move(file);

    return true;
}

void YUVSequenceReader::readComponent(uint8_t *pData, bool isLuma)
{
    const uint32_t div = (uint32_t) (isLuma ? 1 : 2);
    uint32_t height = m_dim.height / div;
    size_t width = m_dim.width / div;
    ptrdiff_t stride = m_stride / (ptrdiff_t) div;

    for (uint32_t i = 0; i < height; i++)
    {
        if (fread(pData + i * stride, sizeof(uint8_t), width, m_file.get()) != width)
            throw EOFException();
    }
}


void YUVSequenceReader::readPicture(uint8_t *pY, uint8_t *pU, uint8_t *pV)
{
    readComponent(pY, true);
    readComponent(pU, false);
    readComponent(pV, false);
}


bool YUVSequenceReader::eof(void)
{
    return (feof(m_file.get()) != 0);
}


int YUVSequenceReader::nframes(void)
{
    struct stat buf;
    stat(m_filename.c_str(), &buf);

    int picsize = m_dim.width * m_dim.height * 3 / 2;
    return (buf.st_size / picsize);
}
