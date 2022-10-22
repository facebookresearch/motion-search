
/*
 Copyright (c) Meta Platforms, Inc. and affiliates.

 This source code is licensed under the BSD3 license found in the
 LICENSE file in the root directory of this source tree.
 */

#include "moments.h"

#include "cpu.h"
#include "cpu_disp.h"

#if defined(_X86) || defined(_X64)

IMPL_PROC_1(int, fastSAD16, (FAST_SAD_FORMAL_ARGS), (FAST_SAD_ACTUAL_ARGS),
            sse2)
IMPL_PROC_1(int, fastSAD8, (FAST_SAD_FORMAL_ARGS), (FAST_SAD_ACTUAL_ARGS), sse2)
IMPL_PROC_1(int, fastSAD4, (FAST_SAD_FORMAL_ARGS), (FAST_SAD_ACTUAL_ARGS), sse2)

IMPL_PROC_1(int, fast_variance16, (FAST_VARIANCE_FORMAL_ARGS),
            (FAST_VARIANCE_ACTUAL_ARGS), sse2)
IMPL_PROC_1(int, fast_variance8, (FAST_VARIANCE_FORMAL_ARGS),
            (FAST_VARIANCE_ACTUAL_ARGS), sse2)
IMPL_PROC_1(int, fast_variance4, (FAST_VARIANCE_FORMAL_ARGS),
            (FAST_VARIANCE_ACTUAL_ARGS), sse2)

IMPL_PROC_1(int, fast_calc_mse16, (FAST_MSE_FORMAL_ARGS),
            (FAST_MSE_ACTUAL_ARGS), sse2)
IMPL_PROC_1(int, fast_calc_mse8, (FAST_MSE_FORMAL_ARGS), (FAST_MSE_ACTUAL_ARGS),
            sse2)
IMPL_PROC_1(int, fast_calc_mse4, (FAST_MSE_FORMAL_ARGS), (FAST_MSE_ACTUAL_ARGS),
            sse2)

IMPL_PROC_1(int, fast_bidir_mse16, (FAST_BIDIR_MSE_FORMAL_ARGS),
            (FAST_BIDIR_MSE_ACTUAL_ARGS), sse2)
IMPL_PROC_1(int, fast_bidir_mse8, (FAST_BIDIR_MSE_FORMAL_ARGS),
            (FAST_BIDIR_MSE_ACTUAL_ARGS), sse2)
IMPL_PROC_1(int, fast_bidir_mse4, (FAST_BIDIR_MSE_FORMAL_ARGS),
            (FAST_BIDIR_MSE_ACTUAL_ARGS), sse2)

#elif defined(_ARM) || defined(_ARM64)

IMPL_PROC_0(int, fastSAD16, (FAST_SAD_FORMAL_ARGS), (FAST_SAD_ACTUAL_ARGS))
IMPL_PROC_0(int, fastSAD8, (FAST_SAD_FORMAL_ARGS), (FAST_SAD_ACTUAL_ARGS))
IMPL_PROC_0(int, fastSAD4, (FAST_SAD_FORMAL_ARGS), (FAST_SAD_ACTUAL_ARGS))

IMPL_PROC_0(int, fast_variance16, (FAST_VARIANCE_FORMAL_ARGS),
            (FAST_VARIANCE_ACTUAL_ARGS))
IMPL_PROC_0(int, fast_variance8, (FAST_VARIANCE_FORMAL_ARGS),
            (FAST_VARIANCE_ACTUAL_ARGS))
IMPL_PROC_0(int, fast_variance4, (FAST_VARIANCE_FORMAL_ARGS),
            (FAST_VARIANCE_ACTUAL_ARGS))

IMPL_PROC_0(int, fast_calc_mse16, (FAST_MSE_FORMAL_ARGS),
            (FAST_MSE_ACTUAL_ARGS))
IMPL_PROC_0(int, fast_calc_mse8, (FAST_MSE_FORMAL_ARGS), (FAST_MSE_ACTUAL_ARGS))
IMPL_PROC_0(int, fast_calc_mse4, (FAST_MSE_FORMAL_ARGS), (FAST_MSE_ACTUAL_ARGS))

IMPL_PROC_0(int, fast_bidir_mse16, (FAST_BIDIR_MSE_FORMAL_ARGS),
            (FAST_BIDIR_MSE_ACTUAL_ARGS))
IMPL_PROC_0(int, fast_bidir_mse8, (FAST_BIDIR_MSE_FORMAL_ARGS),
            (FAST_BIDIR_MSE_ACTUAL_ARGS))
IMPL_PROC_0(int, fast_bidir_mse4, (FAST_BIDIR_MSE_FORMAL_ARGS),
            (FAST_BIDIR_MSE_ACTUAL_ARGS))

#else // generic platform

IMPL_PROC_0(int, fastSAD16, (FAST_SAD_FORMAL_ARGS), (FAST_SAD_ACTUAL_ARGS))
IMPL_PROC_0(int, fastSAD8, (FAST_SAD_FORMAL_ARGS), (FAST_SAD_ACTUAL_ARGS))
IMPL_PROC_0(int, fastSAD4, (FAST_SAD_FORMAL_ARGS), (FAST_SAD_ACTUAL_ARGS))

IMPL_PROC_0(int, fast_variance16, (FAST_VARIANCE_FORMAL_ARGS),
            (FAST_VARIANCE_ACTUAL_ARGS))
IMPL_PROC_0(int, fast_variance8, (FAST_VARIANCE_FORMAL_ARGS),
            (FAST_VARIANCE_ACTUAL_ARGS))
IMPL_PROC_0(int, fast_variance4, (FAST_VARIANCE_FORMAL_ARGS),
            (FAST_VARIANCE_ACTUAL_ARGS))

IMPL_PROC_0(int, fast_calc_mse16, (FAST_MSE_FORMAL_ARGS),
            (FAST_MSE_ACTUAL_ARGS))
IMPL_PROC_0(int, fast_calc_mse8, (FAST_MSE_FORMAL_ARGS), (FAST_MSE_ACTUAL_ARGS))
IMPL_PROC_0(int, fast_calc_mse4, (FAST_MSE_FORMAL_ARGS), (FAST_MSE_ACTUAL_ARGS))

IMPL_PROC_0(int, fast_bidir_mse16, (FAST_BIDIR_MSE_FORMAL_ARGS),
            (FAST_BIDIR_MSE_ACTUAL_ARGS))
IMPL_PROC_0(int, fast_bidir_mse8, (FAST_BIDIR_MSE_FORMAL_ARGS),
            (FAST_BIDIR_MSE_ACTUAL_ARGS))
IMPL_PROC_0(int, fast_bidir_mse4, (FAST_BIDIR_MSE_FORMAL_ARGS),
            (FAST_BIDIR_MSE_ACTUAL_ARGS))

#endif
