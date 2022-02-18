//****************************************************************************
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//  AVIReader.h
//
//  Declaration of the CAVIFileReader class.
//
//  This is a utility class that the sample uses to open and read from a .avi
//  audio/video file.
//
//****************************************************************************

#ifndef __AVIREADER_H
#define __AVIREADER_H

#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <tchar.h>
#include <vfw.h>
#include <wmsdk.h>
#include <assert.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

#define FOURCC_DIB      MAKEFOURCC('D','I','B',' ')
#define FOURCC_iyuv     MAKEFOURCC('i','y','u','v')
#define FOURCC_i420     MAKEFOURCC('i','4','2','0')
#define FOURCC_I420     MAKEFOURCC('I','4','2','0')
#define FOURCC_YV12     MAKEFOURCC('Y','V','1','2')
#define FOURCC_IYUV     MAKEFOURCC('I','Y','U','V')

#ifndef SAFE_DELETE
#define SAFE_DELETE(x) if (x) { delete x; x = NULL;}
#endif

#ifndef SAFE_ARRAY_DELETE
#define SAFE_ARRAY_DELETE(x) if (x) { delete [] x; x = NULL;}
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) if (x) { x->Release(); x = NULL;}
#endif

#ifndef SAFE_CLOSE_FILEHANDLE
#define SAFE_CLOSE_FILEHANDLE(x) if (x) { CloseHandle(x); x = NULL;}
#endif

class CAVIFileReader 
{
public:
    CAVIFileReader();
    HRESULT Init(WCHAR *pwszFileName, double dFRDenominator, double dFRNumerator) ;
    HRESULT Destroy();
    HRESULT ReadFrame_Video(BYTE **pbVideo, DWORD *pdwRawVideoLen, INT64 *pqwTimestamp) ;
    HRESULT ReadFrame_Audio(BYTE **pbAudio, DWORD *pdwRawAudioLen, INT64 *pqwTimestamp);
    HRESULT GetFrameCount(INT32 *piFrameCount); 
    HRESULT GetFirstFrame(INT32 *piFirstFrame);
    HRESULT GetFirstAudio(DWORD *pdwFirstSample);
    HRESULT GetFormat_Audio(WAVEFORMATEX** ppWFX);
    HRESULT GetFormat_Video(BITMAPINFOHEADER** ppBIH);      
    void SeekFirst();

private:
    INT64 GetTimestamp_Audio();
    INT64 GetTimestamp_Video(INT64  i64FrameIndex);  

    PAVIFILE m_aviFileIn; // Pointer to the AVI file.   
    AVIFILEINFO m_aviFileInInfo;  // File info struct.
    AVISTREAMINFO m_streamInfoVideo;  // Video stream info struct.
    AVISTREAMINFO m_streamInfoAudio;  // Audio stream info struct.
    PAVISTREAM m_videoStream;   // Pointer to the current video stream.
    PAVISTREAM m_audioStream;   // Pointer to the current audio stream.
    LPWAVEFORMATEX m_pWavInfoHdr ;       // Pointer to the audio header. (WAVEFORMATEX).
    LPBITMAPINFOHEADER m_pbiHeaderIn ;  //  Pointer to the video header. (BITMAPINFOHEADER)  
    DWORD m_dwFrames;   // Video frame count.
    DWORD m_dwFirstFrame; // First frame number.
    int m_iAudioBufferSize;   // Audio buffer size, in bytes.
    int m_iAudioSamplesPerFrame;  // Count of audio samples per frame.
    BOOL m_bVideoStream;    // Flag, true indicating a video stream exists.
    BOOL m_bAudioStream;    // Flag, true indicating an audio stream exists.  
    BYTE *m_pVideoBufferIn; // Pointer to the video input buffer.  
    BYTE *m_pAudioBufferIn; // Pointer to the audio input buffer.
    DWORD m_dwAudioSamplesPos;  // Current audio position, in samples.
    DWORD m_dwFirstAudio; // First audio sample number.
    WCHAR *m_pwszFileName;  // The path the AVI file.
    double m_dFRDenominator;  // Frame rate denominator. 
    double m_dFRNumerator;    // Frame rate numerator.
    INT32 m_iCurrentFrameNumber; // Keeps track of the current frame number across multiple video frame reads.
    BOOL m_bSwapUV;				// Nick
    LONG m_lYSample;			// Nick
    LONG m_lUVSample;		// Nick
    DWORD m_dwLastAudio; // Nick

};

#endif