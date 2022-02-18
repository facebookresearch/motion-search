#pragma once
#include "ivideosequencereader.h"
class BaseVideoSequenceReader : public IVideoSequenceReader
{
public:
	BaseVideoSequenceReader(void);

	virtual ~BaseVideoSequenceReader(void);

	void read(uint8_t *pY, uint8_t *pU, uint8_t *pV);

	int count() { return m_count; }

protected:
	virtual void readPicture (uint8_t *pY, uint8_t *pU, uint8_t *pV) = 0;

private:
	int m_count;
};

