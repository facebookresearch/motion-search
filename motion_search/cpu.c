/*
 Copyright (c) Meta Platforms, Inc. and affiliates.

 This source code is licensed under the BSD3 license found in the
 LICENSE file in the root directory of this source tree.
 */

#include "cpu.h"

#if defined(_X86) || defined(_X64)

#if defined(_WINDOWS)
#include <intrin.h>
#elif defined(_LINUX)
#include <cpuid.h>
#include <x86intrin.h>
#endif /* defined(_WINDOWS) */

#elif defined(_ARM) || defined(_ARM64)

#if defined(ANDROID)
#include <cpu-features.h>
#endif /* defined(ANDROID) */

#endif /* defined(_X86) || defined(_X64) */

#if defined(_X86) || defined(_X64)

enum {
  SSE_BIT = 0x02000000,
  SSE2_BIT = 0x04000000,
  SSE3_BIT = 0x00000001,
  SSSE3_BIT = 0x00000200,
  SSE41_BIT = 0x00080000,
  SSE42_BIT = 0x00100000,

  OSXSAVE = 0x08000000,
  AVX_BIT = 0x10000000,
  AVX2_BIT = 0x00000020,
  AVX512F_BIT = 0x00010000,
  AVX512DQ_BIT = 0x00020000,
  AVX512BW_BIT = 0x40000000,

  XMM_STATE = 0x00000002,
  YMM_STATE = 0x00000004
};

static void CpuId(int res[4], const int funcId) {
#if defined(_WINDOWS)
  __cpuid(res, funcId);
#elif defined(_LINUX)
  __cpuid_count(funcId, 0, res[0], res[1], res[2], res[3]);
#endif /* defined(_WINDOWS) */
}

static void CpuIdEx(int res[4], const int funcIdx, const int subFuncIdx) {
#if defined(_WINDOWS)
  __cpuidex(res, funcIdx, subFuncIdx);
#elif defined(_LINUX)
  __cpuid_count(funcIdx, subFuncIdx, res[0], res[1], res[2], res[3]);
#endif /* defined(_WINDOWS) */
}

#if defined(_LINUX)
#if !defined(_XCR_XFEATURE_ENABLED_MASK)
enum { _XCR_XFEATURE_ENABLED_MASK = 0 };
#endif /* !defined(_XCR_XFEATURE_ENABLED_MASK) */
#endif /* defined(_LINUX) */

static eCPUType QueryCpuType(void) {
  eCPUType cpuType = cpuPlain;

  int res[4], eax, ebx, ecx, edx;

  /* get the standard feature set */
  CpuId(res, 1);
  eax = res[0];
  ebx = res[1];
  ecx = res[2];
  edx = res[3];

  if ((OSXSAVE | AVX_BIT) == ((OSXSAVE | AVX_BIT) & ecx)) {
    /* check AVX technologies */
    eax = (int)_xgetbv(_XCR_XFEATURE_ENABLED_MASK);

    if ((XMM_STATE | YMM_STATE) == ((XMM_STATE | YMM_STATE) & eax)) {
      CpuIdEx(res, 7, 0);
      ebx = res[1];

      if (AVX512BW_BIT & ebx) {
        cpuType |= cpu_avx512bw;
      }
      if (AVX512DQ_BIT & ebx) {
        cpuType |= cpu_avx512dq;
      }
      cpuType |= (ebx & AVX2_BIT) ? (cpu_avx2) : (cpu_avx);
    } else {
      cpuType = cpu_sse42;
    }
  } else {
    /* check SIMD technologies */
    if (SSE42_BIT & ecx) {
      cpuType = cpu_sse42;
    } else if (SSE41_BIT & ecx) {
      cpuType = cpu_sse41;
    } else if (SSSE3_BIT & ecx) {
      cpuType = cpu_ssse3;
    } else if (SSE3_BIT & ecx) {
      cpuType = cpu_sse3;
    } else if (SSE2_BIT & edx) {
      cpuType = cpu_sse2;
    } else if (SSE_BIT & edx) {
      cpuType = cpu_sse;
    }
  }

  return cpuType;

} /* eCPUType QueryCpuType() */

#elif defined(_ARM) || defined(_ARM64)

#if defined(__ANDROID__)

static eCPUType QueryCpuType() {
  eCPUType cpuType = cpuPlain;

#if defined(_ARM64)
  if (android_getCpuFeatures() & ANDROID_CPU_ARM64_FEATURE_ASIMD) {
    cpuType = cpu_neon;
  }
#else  /* !defined(_ARM64) */
  if (android_getCpuFeatures() & ANDROID_CPU_ARM_FEATURE_NEON) {
    cpuType = cpu_neon;
  }
#endif /* defined(_ARM64) */

  return cpuType;
}

#elif defined(__APPLE__)

static eCPUType QueryCpuType() { return cpu_neon; }

#endif /* defined(__ANDROID__) */

#else /* an unknown platform */

static eCPUType QueryCpuType() { return cpuPlain; }

#endif /* defined(_X86) || defined(_X64) */

static eCPUType g_cpuType = cpuUnk;

eCPUType GetCpuType(void) {
  if (cpuUnk == g_cpuType) {
    g_cpuType = QueryCpuType();
  }

  return g_cpuType;

} /* eCPUType GetCpuType() */

void SetCpuType(const eCPUType cpuType) { g_cpuType = cpuType; }
