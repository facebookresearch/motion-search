
#include <memory_tools.h>

enum {
    ALIGN = 64
};

extern "C"
void *aligned_malloc(const size_t size) {
    size_t allocSize = size + ALIGN + sizeof(void **);
    void *pAllocated = (uint8_t *) malloc(allocSize);

    if (!pAllocated) {
        return nullptr;
    }

    void *pToReturn = (void *) (((size_t) pAllocated + sizeof(void **) + ALIGN - 1) &
        -ALIGN);
    ((void **) pToReturn)[-1] = pAllocated;

    return pToReturn;
}

extern "C"
void aligned_free(void *p) {
    if (!p) {
        return;
    }

    free(((void **) p)[-1]);
}

namespace memory {

template <>
void Set<uint8_t> (uint8_t *p, const uint8_t v, const size_t numItems) {
    memset(p, v, numItems);
}
template <>
void Set<uint16_t> (uint16_t *p, const uint16_t v, const size_t numItems) {
    for (size_t i = 0; i < numItems; ++i) {
        p[i] = v;
    }
}

} // namespace memory
