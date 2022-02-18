#pragma once

#include "YUVFrame.h"

class MotionVectorField
{
public:
	MotionVectorField(int width, int height, int stride, int padded_height, int blocksize);

	virtual ~MotionVectorField(void);

	int predictSpatial(YUVFrame *pFrm, int *mses, unsigned char *MB_modes);

	int predictTemporal(YUVFrame *pCurFrm, YUVFrame *pRefFrm, int *mses, unsigned char *MB_modes);

	int predictBidirectional(YUVFrame *pCurFrm, YUVFrame *pRefFrm1, YUVFrame *pRefFrm2, MotionVectorField *fwdref, MotionVectorField *bckref, int *mses, unsigned char *MB_modes);

	void reset(void);

	inline int blocksize(void) { return m_blocksize; }

	inline int count_I(void) { return m_count_I; }

	inline int count_P(void) { return m_count_P; }

	inline int count_B(void) { return m_count_B; }

	inline int bits(void) { return m_bits; }

	inline int firstMB(void) { return m_firstMB; }

	inline MV *MVs(void) { return &m_pMVs[m_firstMB]; }

	inline int *SADs(void) { return &m_pSADs[m_firstMB]; }

private:
	MV  *m_pMVs;
	int *m_pSADs;
	int  m_numMVbytes;
	int  m_firstMB;

	int  m_width;
	int  m_height;
	int  m_stride;
	int  m_padded_height;
	int  m_blocksize;

	int m_count_I;
	int m_count_P;
	int m_count_B;
	int m_bits;
};
