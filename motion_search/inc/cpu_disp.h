
#pragma once

#define IMPL_PROC_0(return_t, name, FORMAL_ARGS, ACTUAL_ARGS) \
  /* implement the caller */ \
  return_t name FORMAL_ARGS { \
    return name##_c ACTUAL_ARGS; \
  }

#define IMPL_PROC_1(return_t, name, FORMAL_ARGS, ACTUAL_ARGS, cpu0) \
  /* forward declaration of the init proc */ \
  static return_t name##_init FORMAL_ARGS; \
  /* declare the function pointer */ \
  static return_t (*p##name) FORMAL_ARGS = name##_init; \
  /* implement the dispatcher */ \
  static return_t name##_init FORMAL_ARGS { \
    eCPUType cpuType = GetCpuType(); \
    if (cpu_##cpu0 == (cpu_##cpu0 & cpuType)) { \
      p##name = name##_##cpu0; \
    } else { \
      p##name = name##_c; \
    } \
    return p##name ACTUAL_ARGS; \
  } \
  /* implement the caller */ \
  return_t name FORMAL_ARGS { \
    return p##name ACTUAL_ARGS; \
  }

#define IMPL_PROC_2(return_t, name, FORMAL_ARGS, ACTUAL_ARGS, cpu0, cpu1) \
  /* forward declaration of the init proc */ \
  static return_t name##_init FORMAL_ARGS; \
  /* declare the function pointer */ \
  static return_t (*p##name) FORMAL_ARGS = name##_init; \
  /* implement the dispatcher */ \
  static return_t name##_init FORMAL_ARGS { \
    eCPUType cpuType = GetCpuType(); \
    if (cpu_##cpu1 == (cpu_##cpu1 & cpuType)) { \
      p##name = name##_##cpu1; \
    } else if (cpu_##cpu0 == (cpu_##cpu0 & cpuType)) { \
      p##name = name##_##cpu0; \
    } else { \
      p##name = name##_c; \
    } \
    return p##name ACTUAL_ARGS; \
  } \
  /* implement the caller */ \
  return_t name FORMAL_ARGS { \
    return p##name ACTUAL_ARGS; \
  }

//
// add more dispatchers (_3, _4, etc.) here
//
