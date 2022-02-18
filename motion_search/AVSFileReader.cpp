#include "AVSFileReader.h"
#include "EOFException.h"

#include "common.h"

static auto_ptr<wchar_t> convertToWChar(string const & in)
{
    size_t convertedChars = 0;
	size_t infileLen = in.length() + 1;

	wchar_t *out = new wchar_t [ infileLen ];

	mbstowcs_s(&convertedChars, out, infileLen, in.c_str(), _TRUNCATE);

	return auto_ptr<wchar_t>(out);
}


AVSFileReader::AVSFileReader(string filename) : m_filename(convertToWChar(filename)),
												m_pReader(new CAVIFileReader()),
												m_numRead(0)
{
	m_pReader->Init(m_filename.get(), 1001, 24000);
	m_pReader->GetFrameCount(&m_numFramesInFile);

	BITMAPINFOHEADER *pVideoFormat;
	m_pReader->GetFormat_Video(&pVideoFormat);
	m_width  = pVideoFormat->biWidth;
	m_height = pVideoFormat->biHeight;
	m_stride = m_width + 2*HORIZONTAL_PADDING;
}


AVSFileReader::~AVSFileReader(void)
{
}


void AVSFileReader::readPicture(uint8_t *pY, uint8_t *pU, uint8_t *pV)
{
	BYTE *pVideoData = NULL;
	DWORD dwVideoDataSize = 0;
	INT64 tTimeStampIn = 0;

	// When the AVS file codes the Trim command AVIReader correctly returns
	// the number of frames in the file, but does not return EOF on attempts
	// to read past the specified number of frames.
	if (m_numRead >= m_numFramesInFile)
		throw EOFException();

	HRESULT hr = m_pReader->ReadFrame_Video(&pVideoData, &dwVideoDataSize, &tTimeStampIn);          

	if (FAILED(hr))
		throw EOFException();

	for (int i = 0; i < m_height; i++)
		memcpy(pY + i * m_stride, pVideoData + i * m_width, m_width);

	int lumaSize = m_width * m_height;
	for (int i = 0; i < m_height/2; i++)
		memcpy(pU + (i * m_stride / 2), pVideoData + lumaSize + i * m_width/2, m_width/2);

	int chromaSize = m_width/2 * m_height/2;
	for (int i = 0; i < m_height/2; i++)
		memcpy(pV + (i * m_stride/2), pVideoData + lumaSize + chromaSize + i * m_width/2, m_width/2);

	m_numRead++;
}


bool AVSFileReader::eof(void)
{
	return (m_numRead >= m_numFramesInFile);
}
