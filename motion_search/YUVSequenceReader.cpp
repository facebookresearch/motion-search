#include "YUVSequenceReader.h"
#include "EOFException.h"

#include <sys/stat.h>

YUVSequenceReader::YUVSequenceReader(string fname, int width, int height) :	m_width(width),
																			m_height(height),
																			m_stride(width + 2*HORIZONTAL_PADDING),
																			m_filename(fname),
																			m_pFile(fopen(fname.c_str(), "rb"))
{
}


YUVSequenceReader::~YUVSequenceReader(void)
{
	fclose(m_pFile);
}


void YUVSequenceReader::readComponent(uint8_t *pData, bool isLuma)
{
	int div = isLuma ? 1 : 2;
	int height = m_height / div;
	int width = m_width / div;
	int stride = m_stride / div;

	for (int i = 0; i < height; i++)
	{
		if (fread(pData + i * stride, sizeof(uint8_t), width, m_pFile) != width)
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
	return (feof(m_pFile) != 0);
}


int YUVSequenceReader::nframes(void)
{
	struct stat buf;
	stat(m_filename.c_str(), &buf);

	int picsize = m_width * m_height * 3 / 2;
	return (buf.st_size / picsize);
}

