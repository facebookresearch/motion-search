
/*
 Copyright (c) Meta Platforms, Inc. and affiliates.

 This source code is licensed under the BSD3 license found in the
 LICENSE file in the root directory of this source tree.
 */

#include "memory.h"

#if defined(_MSC_VER)
#include <malloc.h>
#else
#include <cstdlib>
#endif

enum { ALIGN = 64 };

extern "C" void *aligned_malloc(const size_t size) {
#if defined(_MSC_VER)
  return _aligned_malloc(size, ALIGN);
#else
  const auto alignedSize = (size + ALIGN - 1) & -ALIGN;
  return aligned_alloc(ALIGN, alignedSize);
#endif
}

extern "C" void aligned_free(void *p) {
#if defined(_MSC_VER)
  _aligned_free(p);
#else
  std::free(p);
#endif
}

namespace memory {

template <>
void Set<uint8_t>(uint8_t *p, const uint8_t v, const size_t numItems) {
  memset(p, v, numItems);
}
template <>
void Set<uint16_t>(uint16_t *p, const uint16_t v, const size_t numItems) {
  for (size_t i = 0; i < numItems; ++i) {
    p[i] = v;
  }
}

} // namespace memory
