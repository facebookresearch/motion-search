#pragma once

#include "BaseVideoSequenceReader.h"
#include "AVIReader.h"

#include <string>
#include <memory>

using std::auto_ptr;
using std::string;

class AVSFileReader : public BaseVideoSequenceReader
{
public:
	AVSFileReader(string filename);
	~AVSFileReader(void);
	bool eof(void);
	int nframes(void){ return m_numFramesInFile; }
	int width(void)  { return m_width; }
	int height(void) { return m_height; }
	int stride(void) { return m_stride; }

protected:
	void readPicture(uint8_t *pY, uint8_t *pU, uint8_t *pV);

private:
	int m_width;
	int m_height;
	int m_stride;

	int m_numFramesInFile;
	int m_numRead;
	auto_ptr<wchar_t> m_filename;
	auto_ptr<CAVIFileReader> m_pReader;
};

