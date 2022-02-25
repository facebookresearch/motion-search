#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <motion_search/inc/common.h>

int extend_frame(unsigned char *frame_ptr, int stride, int width, int height, int pad_size_x, int pad_size_y);
int boundary_extend(unsigned char *ptr1_Y, unsigned char *ptr1_U, unsigned char *ptr1_V, int stride, int width, int height);

#ifdef __cplusplus
}
#endif
