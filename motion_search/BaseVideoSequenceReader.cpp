#include "BaseVideoSequenceReader.h"


BaseVideoSequenceReader::BaseVideoSequenceReader(void) : m_count(0)
{
}


BaseVideoSequenceReader::~BaseVideoSequenceReader(void)
{
}


void BaseVideoSequenceReader::read(uint8_t *pY, uint8_t *pU, uint8_t *pV)
{
	readPicture(pY, pU, pV);
	m_count++;
}
