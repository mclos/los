/*----------------------------------------------------------------------------/
 /  los -  system module  R0.1
 /-----------------------------------------------------------------------------/
 /
 / Copyright (c) 2014-2017 LP电子,All Rights Reserved.
 /
 / 未经授权，禁止商用。
 /----------------------------------------------------------------------------*/

#include "stdint.h"
uint16_t lcd_width = 240;
uint16_t lcd_height = 320;

 

void win32_draw_point(uint16_t x, uint16_t y, uint32_t color)
{
	 
}
//画任意线
//x0,y0:起点 x1,y1:终点
//color:颜色
void win32_draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color)
{
	uint16_t t;
	int32_t xerr = 0, yerr = 0, delta_x, delta_y, distance;
	int32_t incx, incy, uRow, uCol;

	delta_x = x1 - x0; //计算坐标增量
	delta_y = y1 - y0;
	uRow = x0;
	uCol = y0;
	if (delta_x > 0)
		incx = 1; //设置单步方向
	else if (delta_x == 0)
		incx = 0; //垂直线
	else
	{
		incx = -1;
		delta_x = -delta_x;
	}
	if (delta_y > 0)
		incy = 1;
	else if (delta_y == 0)
		incy = 0; //水平线
	else
	{
		incy = -1;
		delta_y = -delta_y;
	}
	if (delta_x > delta_y)
		distance = delta_x; //选取基本增量坐标轴
	else
		distance = delta_y;
	for (t = 0; t <= distance + 1; t++) //画线输出
	{
		win32_draw_point(uRow, uCol, color); //画点
		xerr += delta_x;
		yerr += delta_y;
		if (xerr > distance)
		{
			xerr -= distance;
			uRow += incx;
		}
		if (yerr > distance)
		{
			yerr -= distance;
			uCol += incy;
		}
	}
}
//画横线
void win32_draw_hline(uint16_t x, uint16_t y, uint16_t w, uint32_t c)
{
	int ex = x + w - 1;
	for (; x <= ex; x++)
		win32_draw_point(x, y, c);
}
//画竖线
void win32_draw_vline(uint16_t x, uint16_t y, uint16_t h, uint32_t c)
{
	int ey = y + h - 1;
	for (; y <= ey; y++)
		win32_draw_point(x, y, c);
}
//画空心矩形
void win32_draw_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t c)
{
	win32_draw_hline(x, y, w, c);
	win32_draw_vline(x, y, h, c);

	win32_draw_hline(x, y + h, w, c);
	win32_draw_vline(x + w, y, h, c);
}

//画空心的的矩形-带圆角
void win32_draw_rect_arc(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t r, uint32_t c)
{
	win32_draw_hline(x + r, y, w - 2 * r, c);
	win32_draw_vline(x, y + r, h - 2 * r, c);

	win32_draw_hline(x + r, y + h, w - 2 * r, c);
	win32_draw_vline(x + w, y + r, h - 2 * r, c);
}

//画填充的矩形
void win32_draw_frect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t c)
{
	int ex = x + w - 1;
	int ey = y + h - 1, y0;
	for (; x <= ex; x++)
		for (y0 = y; y0 <= ey; y0++)
			win32_draw_point(x, y0, c);
}
//有弧度的矩形
//画实心矩形-带圆角的
void win32_draw_frect_arc(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t r, uint32_t c)
{
	if (w > 2 * r)
	{ //w比r小了
		win32_draw_frect(x + r, y, w - 2 * r, r, c);
		win32_draw_frect(x, y + r, w, h - 2 * r, c);
		win32_draw_frect(x + r, y + h - r, w - 2 * r, r, c);
		//4个脚4个园
		/*
		 *  0 - 0
		 *  |||||
		 *  0 - O
		 */
		win32_draw_fcircle(x + r, y + r, r, c);
		win32_draw_fcircle(x + r, y + h - r, r, c);
		win32_draw_fcircle(x + w - r, y + h - r, r, c);
		win32_draw_fcircle(x + w - r, y + r, r, c);
	}
	else
		win32_draw_frect(x, y, w, h, c);
}

//画空心园
void win32_draw_circle(uint16_t x, uint16_t y, uint16_t r, uint32_t c)
{
	int32_t D;	 /* Decision Variable */
	uint32_t CurX; /* Current X Value */
	uint32_t CurY; /* Current Y Value */
	D = 3 - (r << 1);
	CurX = 0;
	CurY = r;
	D = 3 - (r << 1);
	CurX = 0;
	CurY = r;
	while (CurX <= CurY)
	{
		win32_draw_point(x + CurX, y + CurY, c);
		win32_draw_point(x + CurX, y - CurY, c);
		win32_draw_point(x - CurX, y + CurY, c);
		win32_draw_point(x - CurX, y - CurY, c);
		win32_draw_point(x + CurY, y + CurX, c);
		win32_draw_point(x + CurY, y - CurX, c);
		win32_draw_point(x - CurY, y + CurX, c);
		win32_draw_point(x - CurY, y - CurX, c);
		if (D < 0)
		{
			D += (CurX << 2) + 6;
		}
		else
		{
			D += ((CurX - CurY) << 2) + 10;
			CurY--;
		}
		CurX++;
	}
}
//画实心园
void win32_draw_fcircle(uint16_t x0, uint16_t y0, uint16_t r, uint32_t c)
{
	uint32_t i;
	uint32_t imax = ((uint32_t)r * 707) / 1000 + 1;
	uint32_t sqmax = (uint32_t)r * (uint32_t)r + (uint32_t)r / 2;
	uint32_t x = r;
	win32_draw_hline(x0 - r, y0, 2 * r, c);
	for (i = 1; i <= imax; i++)
	{
		if ((i * i + x * x) > sqmax)
		{
			// draw lines from outside
			if (x > imax)
			{
				win32_draw_hline(x0 - i + 1, y0 + x, 2 * (i - 1), c);
				win32_draw_hline(x0 - i + 1, y0 - x, 2 * (i - 1), c);
			}
			x--;
		}
		// draw lines from inside (center)
		win32_draw_hline(x0 - x, y0 + i, 2 * x, c);
		win32_draw_hline(x0 - x, y0 - i, 2 * x, c);
	}
}
void win32_draw_arc(uint16_t x0, uint16_t y0, uint16_t r, uint16_t s, uint32_t c)
{
	uint32_t i;
	uint32_t imax = ((uint32_t)r * 707) / 1000 + 1;
	uint32_t sqmax = (uint32_t)r * (uint32_t)r + (uint32_t)r / 2;
	uint32_t x = r;
	win32_draw_hline(x0 - r, y0, 2 * r, c);
	for (i = 1; i <= imax; i++)
	{
		if ((i * i + x * x) > sqmax)
		{
			// draw lines from outside
			if (x > imax)
			{
				win32_draw_hline(x0 - i + 1, y0 + x, 2 * (i - 1), c);
				win32_draw_hline(x0 - i + 1, y0 - x, 2 * (i - 1), c);
			}
			x--;
		}
		// draw lines from inside (center)
		win32_draw_hline(x0 - x, y0 + i, 2 * x, c);
		win32_draw_hline(x0 - x, y0 - i, 2 * x, c);
	}
}
/**
 *椭圆
 */
void win32_draw_ellipse(uint16_t x, uint16_t y, uint16_t a, uint16_t b, uint32_t color)
{
	uint16_t wx, wy;
	uint16_t thresh;
	uint16_t asq = a * a;
	uint16_t bsq = b * b;
	uint16_t xa, ya;

	win32_draw_point(x, y + b, color);
	win32_draw_point(x, y - b, color);

	wx = 0;
	wy = b;
	xa = 0;
	ya = asq * 2 * b;
	thresh = asq / 4 - asq * b;

	for (;;)
	{
		thresh += xa + bsq;

		if (thresh >= 0)
		{
			ya -= asq * 2;
			thresh -= ya;
			wy--;
		}

		xa += bsq * 2;
		wx++;

		if (xa >= ya)
			break;

		win32_draw_point(x + wx, y - wy, color);
		win32_draw_point(x - wx, y - wy, color);
		win32_draw_point(x + wx, y + wy, color);
		win32_draw_point(x - wx, y + wy, color);
	}

	win32_draw_point(x + a, y, color);
	win32_draw_point(x - a, y, color);

	wx = a;
	wy = 0;
	xa = bsq * 2 * a;

	ya = 0;
	thresh = bsq / 4 - bsq * a;

	for (;;)
	{
		thresh += ya + asq;

		if (thresh >= 0)
		{
			xa -= bsq * 2;
			thresh = thresh - xa;
			wx--;
		}
		ya += asq * 2;
		wy++;
		if (ya > xa)
			break;

		win32_draw_point(x + wx, y - wy, color);
		win32_draw_point(x - wx, y - wy, color);
		win32_draw_point(x + wx, y + wy, color);
		win32_draw_point(x - wx, y + wy, color);
	}
}

void win32_send_buf(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *c)
{
	uint16_t i, j;
	for (i = 0; i < h; i++)
		for (j = 0; j < w; j++)
			win32_draw_point(x + j, y + i, c[i * w + j]);
}

void los_draw_point_los(void *los)
{
	uint16_t x = los_get_u16(los, 1);
	uint16_t y = los_get_u32(los, 2);
	uint32_t c = los_get_u32(los, 3);
	win32_draw_point(x, y, c);
}
void los_read_point_los(void *los)
{
	uint16_t x = los_get_u16(los, 1);
	uint16_t y = los_get_u32(los, 2);
	uint32_t color;
	color = lcd_buf[x + win_width * y];
	los_return(los, color);
}
void los_draw_line_los(void *los)
{
	uint16_t x = los_get_u16(los, 1);
	uint16_t y = los_get_u16(los, 2);
	uint16_t x1 = los_get_u16(los, 3);
	uint16_t y1 = los_get_u16(los, 4);
	uint32_t c = los_get_u32(los, 5);
	win32_draw_line(x, y, x1, y1, c);
}
void los_draw_hline_los(void *los)
{
	uint16_t x = los_get_u16(los, 1);
	uint16_t y = los_get_u16(los, 2);
	uint16_t w = los_get_u16(los, 3);
	uint32_t c = los_get_u32(los, 4);
	win32_draw_hline(x, y, w, c);
}
void los_draw_vline_los(void *los)
{
	uint16_t x = los_get_u16(los, 1);
	uint16_t y = los_get_u16(los, 2);
	uint16_t w = los_get_u16(los, 3);
	uint32_t c = los_get_u32(los, 4);
	win32_draw_vline(x, y, w, c);
}
void los_draw_rect_los(void *los)
{
	uint16_t x = los_get_u16(los, 1);
	uint16_t y = los_get_u16(los, 2);
	uint16_t x1 = los_get_u16(los, 3);
	uint16_t y1 = los_get_u16(los, 4);
	uint32_t c = los_get_u32(los, 5);
	win32_draw_rect(x, y, x1, y1, c);
}
void los_draw_frect_los(void *los)
{
	uint16_t x = los_get_u16(los, 1);
	uint16_t y = los_get_u16(los, 2);
	uint16_t x1 = los_get_u16(los, 3);
	uint16_t y1 = los_get_u16(los, 4);
	uint32_t c = los_get_u32(los, 5);
	win32_draw_frect(x, y, x1, y1, c);
}
void los_draw_circle_los(void *los)
{
	uint16_t x = los_get_u16(los, 1);
	uint16_t y = los_get_u16(los, 2);
	uint16_t w = los_get_u16(los, 3);
	uint32_t c = los_get_u32(los, 4);
	win32_draw_circle(x, y, w, c);
}
void los_draw_fcircle_los(void *los)
{
	uint16_t x = los_get_u16(los, 1);
	uint16_t y = los_get_u16(los, 2);
	uint16_t w = los_get_u16(los, 3);
	uint32_t c = los_get_u32(los, 4);
	win32_draw_fcircle(x, y, w, c);
}
void los_draw_buf_los(void *los)
{
}
void los_read_buf_los(void *los) {}
void los_get_windowsize_los(uint16_t *w, uint16_t *h)
{
	*w = lcd_width;
	*h = lcd_height;
}
