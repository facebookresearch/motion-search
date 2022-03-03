
#include <motion_search/inc/BaseVideoSequenceReader.h>

void BaseVideoSequenceReader::read(uint8_t *pY, uint8_t *pU, uint8_t *pV)
{
    readPicture(pY, pU, pV);
    m_count++;
}
