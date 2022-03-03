#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <motion_search/inc/common.h>

void extend_frame(uint8_t *frame_ptr, const ptrdiff_t stride, const DIM dim,
    int pad_size_x, int pad_size_y);

#ifdef __cplusplus
}
#endif
