
/*
 Copyright (c) Meta Platforms, Inc. and affiliates.

 This source code is licensed under the BSD3 license found in the
 LICENSE file in the root directory of this source tree.
 */

#pragma once

#if !defined(__MOTION_SEARCH_INTRINSIC_H)
#define __MOTION_SEARCH_INTRINSIC_H

#include "xplatform.h"

/*
    define useful intrinsic stuff here
*/

#if defined(_X86) || defined(_X64)

#if defined(_MSC_VER)
/* disable warning 'found Intel(R) Advanced Vector Extensions; consider using
 * /arch:AVX' */
#pragma warning(disable : 4752)
#endif /* defined(_MSC_VER) */

/*
    x86 platform
*/

#if defined(_WINDOWS)
#include <intrin.h>
#elif defined(_LINUX)
#include <x86intrin.h>
#else /* unknown platform */
#error PLATFORM IS NOT SET
#endif /* defined(_WINDOWS) */

#elif defined(ARM) || defined(ARM64)

/*
    ARM platform
*/

#include <arm_neon.h>

#endif /* defined(_X86) || defined(_X64) */

#endif /* __MOTION_SEARCH_INTRINSIC_H */
