# SIMD Optimization Guide

## Introduction

An optimized function typically consists of the following parts:

* The main function to call. Please, avoid calling optimized function or
  function pointers directly. It makes code hard to support. Ex. `calc_algo`

* C version. Usually this function is used as a reference for testing
  optimized versions. Using extern "C" declaration helps inter-compiler
  interactions. Ex. `calc_algo_c`

* SIMD optimized version(s). Written with intrinsics or bare asm.
  Using extern "C" declaration helps inter-compiler interactions. 
  Ex. `calc_algo_sse2`

* A function for dynamic CPU dispatching CPU and optimization selection.
  A pretty simple function that checks CPU capabilities and selects the most 
  suitable optimized function. The function is called once at the first call
  of A. function.
  Ex. `calc_algo_disp`

* A function-pointer to most suitable optimization. It's set to the dispatcher
  function at program initialization and later is set once to the most
  suitable optimization.

* Function for getting CPU capabilities.

Most of the parts above are implemented with macros and there is no need to implement
them for every function.

## How to write optimization(s)

* Declare a main function to call. It's recommended to implement 2 defines for
  function's paramters to avoid code duplications:
  ```C
  #define CALC_ALGO_FORMAL_ARGS void *p, const ptrdiff_t stride
  #define CALC_ALGO_ACTUAL_ARGS p, stride
  int calc_algo(CALC_ALGO_FORMAL_ARGS);
  ```
    
* Declare the C version function.
  ```C
  int calc_algo_c(CALC_ALGO_FORMAL_ARGS);
  ```

* Declare available optimizations. Hide declaration under a platform define.
  The function suffix should match a cpu suffix from motion_search/inc/cpu.h
  file.

  ```C
  #if defined(_X86) || defined(_X64)
  int calc_algo_sse2(CALC_ALGO_FORMAL_ARGS);
  #elif defined(_ARM) || defined(_ARM64)
  int calc_algo_neon(CALC_ALGO_FORMAL_ARGS);
  #endif
  ```

    Implement optimized functions in a file that has the platform and the suffix
    in the file name. Some compilers require different compilers flags for
    different optimizations. Having platform/suffix in the file name makes
    finding and grouping files with optimizations easier.
    Hide optimized functions under a platform define. Add a corresponding compiling
    option to CMakeLists.txt file.
    ```
    asm/calc_algo.x86.sse2.asm 
    asm/calc_algo.arm.neon.asm
    ```

* Implement a dispatcher, use exisinting defines from motion_search/inc/cpu_disp.h
  file.

  ```C
  #if defined(_X86) || defined(_X64)
  IMPL_PROC_1(int, calc_algo,  (CALC_ALGO_FORMAL_ARGS), (CALC_ALGO_ACTUAL_ARGS), sse2)
  #elif defined(_ARM) || defined(_ARM64)
  IMPL_PROC_1(int, calc_algo,  (CALC_ALGO_FORMAL_ARGS), (CALC_ALGO_ACTUAL_ARGS), neon)
  #else
  IMPL_PROC_0(int, calc_algo,  (CALC_ALGO_FORMAL_ARGS), (CALC_ALGO_ACTUAL_ARGS))
  #endif
  ```

  There is no limitation on optimizations. List all available optimizations in
  order from lower to higher. The CPU dispatching mechanism picks up the most
  suitable one. `IMPL_PROC_?` number means the number of available optimizations.
  ```C
  IMPL_PROC_5(int, calc_algo,  (CALC_ALGO_FORMAL_ARGS), (CALC_ALGO_ACTUAL_ARGS), sse2, ssse3, sse41, avx2, avx512bwdq)
  ```
  If a required `IMPL_PROC_?` macro is missing, just implement one more by analogue.

## Appendix

* Available platform defines:
  * `_X86` - x86 32-bit platform
  * `_X64` - x86-64 64-bit platform
  * `_ARM` - ARM 32-bit platform (v7a)
  * `_ARM64` - ARM 64-bit platform (v8a)

* Source files platform suffixes:
  * `.x86.` - both x86 and x86-64 platforms
  * `.x64.` - x86-64 platform
  * `.arm.` - both 32- and 64-bit ARM platforms
  * `.arm64.` - 64-bit ARM platform

Yes, there is a little inconsistency between platform defines and platform suffixes.