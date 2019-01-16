//Copyright (c) 2014 LP

#ifndef _lcdDriver_H
#define _lcdDriver_H

#include "stdint.h"
typedef struct _ul_init_t //ul的底层驱动等信息 也是当前界面的事件触发
{
    uint16_t w;
    uint16_t h;
    /**
   *这些参数是外部提供的
   */
    uint8_t point_key;        //0是touch 操作，1是key操作
    int8_t key_flgs;          //按下了，还是担起    0是按下了 1是担起
    uint8_t key_key;          //按键号
    int8_t point_flgs;        //鼠标点击了
    int16_t point_x, point_y; //点击坐标

    uint8_t *font; //当前系统字库
} lgui_t;

#define lcd_draw_buf lcd_send_buf
unsigned short RGB888ToRGB565(unsigned int n888Color);
void check_lgui_event(void);
 
void lgui_lcd_init(void);
void lcd_draw_point(uint16_t x, uint16_t y, uint32_t c);
uint32_t lcd_read_point(uint16_t x, uint16_t y);
void lcd_draw_buf(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *c);
//画线
void lcd_draw_line(uint16_t x, uint16_t y, uint16_t ex, uint16_t ey, uint32_t c);
//画横线
void lcd_draw_hline(uint16_t x, uint16_t y, uint16_t w, uint32_t c);
//画竖线
void lcd_draw_vline(uint16_t x, uint16_t y, uint16_t h, uint32_t c);
//画空心矩形
void lcd_draw_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t c);
void lcd_draw_rect_arc(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t r, uint32_t c);

//画填充的矩形
void lcd_draw_frect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t c);
void lcd_draw_frect_arc(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t r, uint32_t c);
//画空心园
void lcd_draw_circle(uint16_t x, uint16_t y, uint16_t r, uint32_t c);
//画实心园
void lcd_draw_fcircle(uint16_t x, uint16_t y, uint16_t r, uint32_t c);
lgui_t* los_get_io(void);
#endif /* _lcdDriver_H */
