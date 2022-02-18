#pragma once

#include "common.h"
#include "BaseVideoSequenceReader.h"

#include <cstdio>
#include <string>

using std::string;

class YUVSequenceReader : public BaseVideoSequenceReader
{
public:
	YUVSequenceReader (string fname, int width, int height);
	~YUVSequenceReader (void);
	bool eof(void);
	int nframes(void);
	int width(void)  { return m_width; }
	int height(void) { return m_height; }
	int stride(void) { return m_stride; }

protected:
	void readPicture(uint8_t *pY, uint8_t *pU, uint8_t *pV);

private:
	const int m_width;
	const int m_height;
	const int m_stride;

	const string m_filename;
	FILE *m_pFile;

	void readComponent(uint8_t *pData, bool isLuma);
};

