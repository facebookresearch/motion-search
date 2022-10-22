
/*
 Copyright (c) Meta Platforms, Inc. and affiliates.

 This source code is licensed under the BSD3 license found in the
 LICENSE file in the root directory of this source tree.
 */

#pragma once

#if !defined(__SSIM_XPLATFORM_H)
#define __SSIM_XPLATFORM_H

/* define the OS identifier */
#if defined(_WIN32) || defined(_WIN64)

#ifndef _WINDOWS
#define _WINDOWS
#endif /* _WINDOWS */

#define UNUSED(p) (p)

#elif defined(__linux__) || defined(__APPLE__)

#ifndef _LINUX
#define _LINUX
#endif /* _LINUX */

#define UNUSED(p) (void)(p)

#endif /* OS platform */

/* define the platform identifier */
#if defined(_M_X64) || defined(_M_AMD64) || defined(__amd64__) ||              \
    defined(__x86_64__)

/* declare Intel x64 / AMD64 platfrom */
#ifndef _X64
#define _X64
#endif /* _X64 */

#elif defined(_M_IX86) || defined(__i386__) || defined(_X86_)

/* declare Intel x86 platfrom */
#ifndef _X86
#define _X86
#endif /* _X86 */

#elif defined(_M_ARM64) || defined(ARM64) || defined(_ARM64) ||                \
    defined(__arm64__) || defined(__aarch64__)

/* declare ARM64 platfrom */
#ifndef _ARM64
#define _ARM64
#endif /* _ARM64 */

#elif defined(_M_ARM) || defined(ARM) || defined(_ARM) || defined(__arm__)

/* declare ARM platfrom */
#ifndef _ARM
#define _ARM
#endif /* _ARM */

#endif /* CPU platform */

#endif /* !defined(__SSIM_XPLATFORM_H) */
