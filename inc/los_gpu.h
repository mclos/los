#ifndef LOS_GPU_API_H_
#define LOS_GPU_API_H_

#include <stdint.h>
void los_get_windowsize_los(uint16_t *w, uint16_t *h);
void los_draw_point_los(void *los);
void los_read_point_los(void *los);
void los_draw_line_los(void *los);
void los_draw_hline_los(void *los);
void los_draw_vline_los(void *los);
void los_draw_rect_los(void *los);
void los_draw_frect_los(void *los);
void los_draw_circle_los(void *los);
void los_draw_fcircle_los(void *los);
void los_draw_buf_los(void *los);
void los_read_buf_los(void *los);
#endif
