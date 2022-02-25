#include "ComplexityAnalyzer.h"
#include "EOFException.h"

#include "frame.h"
#include <motion_search/inc/moments.h>
#include "motion_search.h"


ComplexityAnalyzer::ComplexityAnalyzer(IVideoSequenceReader *reader) :
	m_width(reader->width()),
	m_height(reader->height()),
	m_stride(reader->width() + 2*HORIZONTAL_PADDING),
	m_padded_height(reader->height() + 2*VERTICAL_PADDING),
	m_GOP_size(48),
	m_subGOP_size(3),
	m_pReader(reader),
	m_pReorderedInfo(NULL)
{
	m_GOP_error = 0;
	m_GOP_bits  = 0;
	m_GOP_count = 0;
	m_SGOP_bits = 0;

	for (int i = 0; i <= m_subGOP_size; i++)
		pics.push_back(new YUVFrame(m_pReader));

	m_pPmv  = new MotionVectorField(m_width, m_height, m_stride, m_padded_height, MB_WIDTH);
	m_pB1mv = new MotionVectorField(m_width, m_height, m_stride, m_padded_height, MB_WIDTH);
	m_pB2mv = new MotionVectorField(m_width, m_height, m_stride, m_padded_height, MB_WIDTH);

	int stride_MB = m_width/MB_WIDTH+2;
	int padded_height_MB = (m_height+MB_WIDTH-1)/MB_WIDTH+2;

	m_mses = memory::AlignedAlloc<int> ((stride_MB)*(padded_height_MB));
	if(m_mses==NULL)
	{
		printf("Not enough memory (%zu bytes) for %s\n",(stride_MB)*(padded_height_MB)*sizeof(int),"m_mses");
		exit(-1);
	}
	m_MB_modes = memory::AlignedAlloc<unsigned char> ((stride_MB)*(padded_height_MB));
	if(m_MB_modes==NULL)
	{
		printf("Not enough memory (%zu bytes) for %s\n",(stride_MB)*(padded_height_MB)*sizeof(unsigned char),"m_MB_modes");
		exit(-1);
	}
}


ComplexityAnalyzer::~ComplexityAnalyzer(void)
{
	pics.clear();

	delete m_pPmv;
	delete m_pB1mv;
	delete m_pB2mv;
}


void ComplexityAnalyzer::reset_gop_start(void)
{
	m_pPmv->reset();
	m_pB1mv->reset();
	m_pB2mv->reset();
}


void ComplexityAnalyzer::add_info (int num, char p, int err, int count_I, int count_P, int count_B, int bits)
{
	complexity_info_t *i = new complexity_info_t;

	// Convert all numbering to 0..N-1
	i->picNum = num-1;
	i->picType = p;
	i->error = err;
	i->count_I = count_I;
	i->count_P = count_P;
	i->count_B = count_B;
	i->bits = bits;

	if (p == 'I' || p == 'P') {
		if (m_pReorderedInfo != NULL)
			m_info.push_back(m_pReorderedInfo);
		m_pReorderedInfo = i;
	}
	else {
		m_info.push_back(i);
	}
}


void ComplexityAnalyzer::process_i_picture(YUVFrame *pict)
{
	reset_gop_start();
	int error = m_pPmv->predictSpatial(pict, &m_mses.get()[m_pPmv->firstMB()], &m_MB_modes.get()[m_pPmv->firstMB()]);
	int bits = m_pPmv->bits();

	//We are weighting I-frames by 10% more bits (282/256), since the QP needs to be the lowest among I/P/B
	bits = (I_FRAME_BIT_WEIGHT*bits+128)>>8;
	m_GOP_bits += bits;
	m_GOP_error += error;
	add_info(m_pReader->count(), 'I', error, 
		m_pPmv->count_I(), 0, 0, bits);
	//Following printout is equivalent to IoK's original .C code
	//printf("Frame %6d (I), I:%6d, P:%6d, B:%6d, MSE = %9d, bits = %7d\n",pict->pos()+1,m_pPmv->count_I(),0,0,error,bits);
	pict->boundaryExtend();
}


void ComplexityAnalyzer::process_p_picture(YUVFrame *pict, YUVFrame *ref)
{
	int error = m_pPmv->predictTemporal(pict, ref, &m_mses.get()[m_pPmv->firstMB()], &m_MB_modes.get()[m_pPmv->firstMB()]);
	int bits = m_pPmv->bits();

	//We are weighting P-frames by 5% more bits (269/256), since the QP needs to be lower than B (but higher than I)
	bits = (P_FRAME_BIT_WEIGHT*bits+128)>>8;
	m_GOP_bits += bits;
	m_GOP_error += error;
	add_info(m_pReader->count(), 'P', error, 
		m_pPmv->count_I(), m_pPmv->count_P(), 0, bits);
	//Following printout is equivalent to IoK's original .C code
	//printf("Frame %6d (P), I:%6d, P:%6d, B:%6d, MSE = %9d, bits = %7d\n",pict->pos()+1,m_pPmv->count_I(),m_pPmv->count_P(),0,error,bits);
	pict->boundaryExtend();
}


void ComplexityAnalyzer::process_b_picture(YUVFrame *pict, YUVFrame *fwdref, YUVFrame *backref)
{
	int error = m_pPmv->predictBidirectional(pict, fwdref, backref, m_pB1mv, m_pB2mv, &m_mses.get()[m_pPmv->firstMB()], &m_MB_modes.get()[m_pPmv->firstMB()]);
	int bits = m_pPmv->bits();

	//We are weighting B-frames by 0% more bits (256/256), since QP needs to be highest among I/P/B
	bits = (B_FRAME_BIT_WEIGHT*bits+128)>>8;
	m_GOP_bits += bits;
	m_GOP_error += error;
	add_info(m_pReader->count() - (backref->pos() - pict->pos()), 'B', error, 
		m_pPmv->count_I(), m_pPmv->count_P(), m_pPmv->count_B(), bits);
	//Following printout is equivalent to IoK's original .C code
	//printf("Frame %6d (B), I:%6d, P:%6d, B:%6d, MSE = %9d, bits = %7d\n",pict->pos()+1,m_pPmv->count_I(),m_pPmv->count_P(),m_pPmv->count_B(),error,bits);
}


void ComplexityAnalyzer::analyze()
{
	int td = 0;
	int td_ref;
	const int SGOP_size = 5;

	try {
		while(!m_pReader->eof())
		{
			printf ("picture #%d\r", m_pReader->count()-1);

			if((m_pReader->count() % m_GOP_size) == 0) {
				if(m_pReader->count())
				{
					printf("GOP: %3d, GOP-bits = %d\n",m_GOP_count,m_GOP_bits);
					m_GOP_count++;
				}
				m_SGOP_bits += m_GOP_bits;
				if((m_GOP_count%SGOP_size)==0)
				{
					if(m_GOP_count)
					{
						printf("10-sec GOP: %3d, bits = %d\n",m_GOP_count/SGOP_size-1,m_SGOP_bits);
					}
					m_SGOP_bits = 0;
				}
				//printf("New GOP\n");
				m_GOP_error = 0;
				m_GOP_bits = 0;

				td = 0;
				pics[0]->readNextFrame();
				process_i_picture(pics[0]);
			}
			else {
				pics[0]->swapFrame(pics[m_subGOP_size]);
			}

			for(td_ref = td; td < (m_GOP_size - 1) && (td - td_ref) < m_subGOP_size; td++) {
				pics[td + 1 - td_ref]->readNextFrame();
			}

			process_p_picture(	/* target    */ pics[td - td_ref],
								/* reference */ pics[0]);

			for(int j = 1; j < td - td_ref; j++) {
				process_b_picture(	/* target  */ pics[j],
									/* forward */ pics[0],
									/* reverse */ pics[td - td_ref]);
			}
		}
	}
	catch (EOFException &e)
	{
		printf ("\n%s\n", e.what());
	}

	if (m_pReorderedInfo != NULL)
		m_info.push_back(m_pReorderedInfo);

	printf("Processed %d frames\n",m_pReader->count());
}
