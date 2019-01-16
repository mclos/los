#ifndef LOS_GPU_API_H_
#define LOS_GPU_API_H_

#include <stdint.h>
void los_get_windowsize(uint16_t *w, uint16_t *h);
void los_draw_point(uint16_t x, uint16_t y, uint32_t c);
uint32_t los_read_point(uint16_t x, uint16_t y);
void los_draw_line(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t c);
void los_draw_hline(uint16_t x, uint16_t y, uint16_t w, uint32_t c);
void los_draw_vline(uint16_t x, uint16_t y, uint16_t h, uint32_t c);
void los_draw_rect(uint16_t x, uint16_t y, uint16_t x1, uint16_t y1,uint32_t c);
void los_draw_frect(uint16_t x, uint16_t y, uint16_t x1, uint16_t y1,uint32_t c);
void los_draw_cycrt(uint16_t x, uint16_t y, uint16_t r, uint32_t c);
void los_draw_fcycrt(uint16_t x, uint16_t y, uint16_t r, uint32_t c);
void los_draw_buf(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *c);
void los_read_buf(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *c);
#endif
