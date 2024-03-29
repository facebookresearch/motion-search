/*
 Copyright (c) Meta Platforms, Inc. and affiliates.

 This source code is licensed under the BSD3 license found in the
 LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "common.h"

#include "IVideoSequenceReader.h"
#include "memory.h"
#include "MotionVectorField.h"

#include <vector>

using std::vector;

typedef struct {
    int picNum;
    int picType;
    int count_I;
    int count_P;
    int count_B;
    int bits;
    int error;
} complexity_info_t;

class ComplexityAnalyzer {
public:
    ComplexityAnalyzer(IVideoSequenceReader *reader, int gop_size, int num_frames, int b_frames);

    ~ComplexityAnalyzer(void);

    void analyze(void);

    vector<complexity_info_t *> getInfo() { return m_info; }

private:
    DIM m_dim;
    int m_stride;
    int m_padded_height;
    int m_num_frames;

    int m_GOP_size;
    int m_subGOP_size;

    int m_GOP_error;
    int m_GOP_bits;
    int m_GOP_count;

    vector<YUVFrame *> pics;

    MotionVectorField *m_pPmv;
    MotionVectorField *m_pB1mv;
    MotionVectorField *m_pB2mv;

    memory::aligned_unique_ptr<int> m_mses;
    memory::aligned_unique_ptr<unsigned char> m_MB_modes;

    IVideoSequenceReader *m_pReader;

    vector<complexity_info_t *> m_info;
    complexity_info_t *m_pReorderedInfo;

    void reset_gop_start(void);

    void add_info(int num, char p, int err, int count_I, int count_P, int count_B, int bits);

    void process_i_picture(YUVFrame *pict);

    void process_p_picture(YUVFrame *pict, YUVFrame *ref);

    void process_b_picture(YUVFrame *pict, YUVFrame *fwdref, YUVFrame *backref);

    ComplexityAnalyzer(ComplexityAnalyzer &) = delete;

    ComplexityAnalyzer &operator=(ComplexityAnalyzer &) = delete;
};
