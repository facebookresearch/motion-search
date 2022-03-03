#include <motion_search/inc/frame.h>

#include <memory.h>

// Pad frame
void extend_frame(uint8_t *frame_ptr, const ptrdiff_t stride, const DIM dim,
    int pad_size_x, int pad_size_y)
{
    int i, j;

    for(i=0;i<dim.height;i++)
    {
        memset(frame_ptr-pad_size_x,frame_ptr[0],pad_size_x);
        memset(frame_ptr+dim.width,frame_ptr[dim.width-1],pad_size_x);
        if(i==0)
        {
            for(j=-pad_size_y;j<0;j++)
            {
                memcpy(frame_ptr+j*stride-pad_size_x,frame_ptr-pad_size_x,stride);
            }
        }
        else if(i==(dim.height-1))
        {
            for(j=1;j<=pad_size_y;j++)
            {
                memcpy(frame_ptr+j*stride-pad_size_x,frame_ptr-pad_size_x,stride);
            }
        }
        frame_ptr += stride;
    }
}
