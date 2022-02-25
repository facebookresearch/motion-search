
#pragma once

#if defined(__cplusplus)

#include <motion_search/inc/common.h>

#include <memory>
#include <type_traits>

extern "C" {
#endif // defined(__cplusplus)

void *aligned_malloc(const size_t size);
void aligned_free(void *p);

#if defined(__cplusplus)
} // extern "C"
#endif // defined(__cplusplus)

#if defined(__cplusplus)

namespace memory {

struct aligned_deallocator {
    template <typename data_t>
    void operator () (data_t *p) {
        static_assert(std::is_trivially_destructible<data_t>::value,
            "objects should have no or a trivial desctructor");
        aligned_free(p);
    }
};

template <typename data_t>
using aligned_unique_ptr = std::unique_ptr<data_t, aligned_deallocator>;

template <typename data_t> inline
aligned_unique_ptr<data_t> AlignedAlloc(const size_t numItems) {
    const size_t size = sizeof(data_t) * numItems;
    std::unique_ptr<data_t, aligned_deallocator> p((data_t *) aligned_malloc(size));
    if constexpr (!std::is_trivial<data_t>::value) {
        // call constructors
        data_t *items = new(p.get()) data_t[numItems];
    }
    return p;
}

template <typename data_t> inline
void Copy(data_t *dst, const data_t *src, const size_t numItems) {
    memcpy(dst, src, sizeof(data_t) * numItems);
}

template <typename data_t>
void Set(data_t *p, const data_t v, const size_t numItems);

} // namespace memory

#endif // defined(__cplusplus)
