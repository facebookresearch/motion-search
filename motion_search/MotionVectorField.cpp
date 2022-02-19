#include "MotionVectorField.h"

#include "motion_search.h"

MotionVectorField::MotionVectorField(int width, int height, int stride, int padded_height, int blocksize) : m_width(width),
																											m_height(height),
																											m_stride(stride),
																											m_padded_height(padded_height),
																											m_blocksize(blocksize),
																											m_firstMB(width/MB_WIDTH+2 + 1),
																											m_count_I(0),
																											m_count_P(0),
																											m_count_B(0)
{
	int i,j;
	int stride_MB = width/MB_WIDTH+2;
	int padded_height_MB = (height+MB_WIDTH-1)/MB_WIDTH+2;
	int num_blocks = stride_MB*padded_height_MB;

	m_numMVbytes  = num_blocks * sizeof(MV);
	m_pMVs  = (MV *) aligned_alloc(m_numMVbytes, 16);
	if (m_pMVs == NULL) {
		printf("Not enough memory (%d bytes) for motion vectors\n", m_numMVbytes);
		exit(-1);
	}

	int num_sad_bytes = num_blocks * sizeof(int);
	m_pSADs  = (int *) aligned_alloc(num_sad_bytes, 16);
	if (m_pSADs == NULL) {
		printf("Not enough memory (%d bytes) for SADs\n", num_sad_bytes);
		exit(-1);
	}

	for(j=0;j<stride_MB;j++)
	{
		m_pSADs[j] = BORDER_SADS;
	}
	for(i=1;i<padded_height_MB-1;i++)
	{
		m_pSADs[i*stride_MB] = m_pSADs[(i+1)*stride_MB-1] = BORDER_SADS;
	}
	for(j=0;j<stride_MB;j++)
	{
		m_pSADs[i*stride_MB+j] = BORDER_SADS;
	}
}


MotionVectorField::~MotionVectorField(void)
{
	free(m_pMVs);
	free(m_pSADs);
}


int MotionVectorField::predictSpatial(YUVFrame *pFrm, int *mses, unsigned char *MB_modes)
{
	return spatial_search(pFrm->y(), pFrm->y(), 
		pFrm->stride(), pFrm->width(), pFrm->height(), m_blocksize, m_blocksize, 
		&m_pMVs[m_firstMB], &m_pSADs[m_firstMB], mses, MB_modes,
		&m_count_I, &m_bits);

}


int MotionVectorField::predictTemporal(YUVFrame *pCurFrm, YUVFrame *pRefFrm, int *mses, unsigned char *MB_modes)
{
	return motion_search(pCurFrm->y(), pRefFrm->y(), 
		pCurFrm->stride(), pCurFrm->width(), pCurFrm->height(), m_blocksize, m_blocksize, 
		&m_pMVs[m_firstMB], &m_pSADs[m_firstMB], mses, MB_modes,
		&m_count_I, &m_count_P, &m_bits);
}


int MotionVectorField::predictBidirectional(YUVFrame *pCurFrm, YUVFrame *pRefFrm1, YUVFrame *pRefFrm2, MotionVectorField *fwdref, MotionVectorField *bckref, int *mses, unsigned char *MB_modes)
{
	int pos = pCurFrm->pos() - pRefFrm1->pos();
	int total = pRefFrm2->pos() - pRefFrm1->pos();
	short td1, td2;

	td1 = (short) ((pos*32768+total/2)/total);
	td2 = (short) (32768 - td1);
	return bidir_motion_search(pCurFrm->y(), pRefFrm1->y(), pRefFrm2->y(), 
		pCurFrm->stride(), pCurFrm->width(), pCurFrm->height(), m_blocksize, m_blocksize, 
		this->MVs(), fwdref->MVs(), bckref->MVs(), 
		fwdref->SADs(), bckref->SADs(), mses, MB_modes, td1, td2,
		&m_count_I, &m_count_P, &m_count_B, &m_bits);
}


void MotionVectorField::reset(void)
{
	memset(m_pMVs, 0, m_numMVbytes);
}
