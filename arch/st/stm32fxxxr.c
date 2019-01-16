/*----------------------------------------------------------------------------/
 /  los -  system module  R0.1
 /-----------------------------------------------------------------------------/
 /
 / Copyright (c) 2014-2017 LP电子,All Rights Reserved.
 /
 / 未经授权，禁止商用。
 /----------------------------------------------------------------------------*/
#if (defined STM32F103xE) || (defined STM32F401xC)
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "los_usart.h"
#ifdef STM32F401xC
#include "stm32f4xx_hal.h"
#else
#include "stm32f1xx_hal.h"
#endif
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern RTC_HandleTypeDef hrtc;
//串口的head
extern los_port_t los_port;
//外部启动注册
static const char *TAG = "stm32_device";
uint8_t los_usart_read_b(uint8_t id, uint8_t *ch)
{
  return 1;
}
uint8_t los_usart_send_b(uint8_t id, uint8_t ch)
{
  if (id == 0)
    return HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
  else
    return HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
}
uint8_t los_usart_send_bs(uint8_t id, uint8_t *ch, uint32_t size)
{
  if (id == 0)
    return HAL_UART_Transmit(&huart2, (uint8_t *)ch, size, 0xFFFF);
  else
    return HAL_UART_Transmit(&huart1, (uint8_t *)ch, size, 0xFFFF);
}
extern void MX_USART1_UART_Init(void);
//外部启动注册
uint8_t los_usart_init(uint8_t id, los_uart_t *los_uart)
{
  logi(TAG, "init uart[%d]", id);
  id--;
  // printf("opesn=%d,%d\r\n", los_port.buf[0], los_port.len[0]);
  if (id == 0)
  {
    huart1.Init.BaudRate = los_uart->prot;
    MX_USART1_UART_Init();
    HAL_UART_MspInit(&huart1);
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
    return HAL_UART_Receive_DMA(&huart1, los_port.buf[id], los_port.len[id]); //直接返回错误
  }
  return 1;
}
/**
 * 2018-12-27,bug，可能关的不切换，执行这个function，然后就出错了
 * 清空标记试试--不行
 * */
uint8_t los_usart_deinit(uint8_t id)
{
  return 0;
  logi(TAG, "UN init %d\r\n", id);
  id--;
  if (id == 0)
  {
    los_port.flg[0] = 0; //清空标记试试
    __HAL_UART_DISABLE_IT(&huart1, UART_IT_IDLE);
    id = HAL_UART_DMAStop(&huart1); //直接返回错误
    HAL_UART_MspDeInit(&huart1);
    return id;
  }
  return 1; //直接返回错误
}
void rtc_set_time(uint8_t hour, uint8_t minute, uint8_t second)
{
  RTC_TimeTypeDef stimestructure;
  stimestructure.Hours = hour;
  stimestructure.Minutes = minute;
  stimestructure.Seconds = second;
  HAL_RTC_SetTime(&hrtc, &stimestructure, RTC_FORMAT_BIN);
}
void rtc_set_data(uint8_t year, uint8_t month, uint8_t day)
{
  RTC_DateTypeDef sdatestructure;
  sdatestructure.Year = year - 2000;
  sdatestructure.Month = month;
  sdatestructure.Date = day;
  HAL_RTC_SetDate(&hrtc, &sdatestructure, RTC_FORMAT_BIN);
}
/**
 * los的time结构体
 */
typedef struct _tm_t
{
  uint8_t sec;   ///< seconds,  range 0 to 59
  uint8_t min;   ///< minutes, range 0 to 59
  uint8_t hour;  ///< hours, range 0 to 23
  uint8_t mday;  ///< day of the month, range 1 to 31
  uint8_t month; ///< month, range 0 to 11
  uint8_t week;  ///< day of the week, range 0 to 6
  uint16_t yday; ///< day in the year, range 0 to 365
  uint16_t year; ///< The number of years since 2000
} tm_t;
uint32_t los_get_time(tm_t *tm)
{
  RTC_DateTypeDef sdatestructure;
  RTC_TimeTypeDef stimestructure;
  HAL_RTC_GetTime(&hrtc, &stimestructure, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&hrtc, &sdatestructure, RTC_FORMAT_BIN);
  tm->sec = stimestructure.Seconds;
  tm->min = stimestructure.Minutes;
  tm->hour = stimestructure.Hours;
  tm->mday = sdatestructure.Date;
  tm->month = sdatestructure.Month;
  tm->year = sdatestructure.Year + 2000;
  tm->week = sdatestructure.WeekDay;
  //printf("%d-%d-%d %d:%d:%d\r\n", tm ->year, tm ->month, tm ->mday, tm ->hour,
  //		tm ->min, tm ->sec);
  return 0;
}
uint32_t los_set_time(tm_t *tm)
{
  RTC_DateTypeDef sdatestructure;
  RTC_TimeTypeDef stimestructure;
  sdatestructure.Year = tm->year - 2000;
  sdatestructure.Month = tm->month;
  sdatestructure.Date = tm->mday;
  stimestructure.Hours = tm->hour;
  stimestructure.Minutes = tm->min;
  stimestructure.Seconds = tm->sec;
  stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;
  // printf("%d-%d\r\n", tm->year, sdatestructure.Year);
  HAL_RTC_SetTime(&hrtc, &stimestructure, RTC_FORMAT_BIN);
  HAL_RTC_SetDate(&hrtc, &sdatestructure, RTC_FORMAT_BIN);
  return 0;
}

/////////////////////////////////以下 tft//////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//Copyright (c) 2014 LP
#include "lcdDriver.h"
#include <stdlib.h>
#include <string.h>
#include "tft_lcd.h"
#define OPEN_WINDOW //有开窗 lcd_clean_window和lcd_send_buf函数必须实现
extern void lcd_send_point(uint16_t x, uint16_t y, uint32_t color);
extern void lcd_clean_window(int x, int y, int w, int h, uint32_t color);
extern void lcd_send_buf(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *c);

void lgui_lcd_init(void)
{
}
void lgui_layer(u8 id)
{
}
void lcd_draw_point(u16 x, u16 y, u32 c)
{
  lcd_send_point(x, y, c);
}
u32 lcd_read_point(u16 x, u16 y)
{
  return 0;
}
//画任意线
//x0,y0:起点 x1,y1:终点
//color:颜色
void lcd_draw_line(u16 x0, u16 y0, u16 x1, u16 y1, u32 color)
{
  u16 t;
  s32 xerr = 0, yerr = 0, delta_x, delta_y, distance;
  s32 incx, incy, uRow, uCol;

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
    lcd_draw_point(uRow, uCol, color); //画点
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
void lcd_draw_hline(u16 x, u16 y, u16 w, u32 c)
{
#ifndef OPEN_WINDOW
  int ex = x + w - 1;
  for (; x <= ex; x++)
    lcd_draw_point(x, y, c);
#else
  lcd_clean_window(x, y, w, 1, c);
#endif
}
//画竖线
void lcd_draw_vline(u16 x, u16 y, u16 h, u32 c)
{
#ifndef OPEN_WINDOW
  int ey = y + h - 1;
  for (; y <= ey; y++)
    lcd_draw_point(x, y, c);
#else
  lcd_clean_window(x, y, 1, h, c);
#endif
}
//画空心矩形
void lcd_draw_rect(u16 x, u16 y, u16 w, u16 h, u32 c)
{
  lcd_draw_hline(x, y, w, c);
  lcd_draw_vline(x, y, h, c);

  lcd_draw_hline(x, y + h, w, c);
  lcd_draw_vline(x + w, y, h, c);
}

//画空心的的矩形-带圆角
void lcd_draw_rect_arc(u16 x, u16 y, u16 w, u16 h, u8 r, u32 c)
{
  lcd_draw_hline(x + r, y, w - 2 * r, c);
  lcd_draw_vline(x, y + r, h - 2 * r, c);

  lcd_draw_hline(x + r, y + h, w - 2 * r, c);
  lcd_draw_vline(x + w, y + r, h - 2 * r, c);
}
#ifndef OPEN_WINDOW
void lcd_draw_buf(u16 x, u16 y, u16 w, u16 h, uint16_t *c)
{
  uint16_t i, j;
  for (i = 0; i < h; i++)
    for (j = 0; j < w; j++)
      lcd_draw_point(x + j, y + i, c[i * w + j]);
}
#else

#define lcd_draw_buf lcd_send_buf

#endif
//画填充的矩形
void lcd_draw_frect(u16 x, u16 y, u16 w, u16 h, u32 c)
{
#ifndef OPEN_WINDOW
  int ex = x + w - 1;
  int ey = y + h - 1, y0;
  for (; x <= ex; x++)
    for (y0 = y; y0 <= ey; y0++)
      lcd_draw_point(x, y0, c);
#else
  lcd_clean_window(x, y, w, h, c);
#endif
}
//有弧度的矩形
//画实心矩形-带圆角的
void lcd_draw_frect_arc(u16 x, u16 y, u16 w, u16 h, u8 r, u32 c)
{
  if (w > 2 * r)
  { //w比r小了
    lcd_draw_frect(x + r, y, w - 2 * r, r, c);
    lcd_draw_frect(x, y + r, w, h - 2 * r, c);
    lcd_draw_frect(x + r, y + h - r, w - 2 * r, r, c);
    //4个脚4个园
    /*
		 *  0 - 0
		 *  |||||
		 *  0 - O
		 */
    lcd_draw_fcircle(x + r, y + r, r, c);
    lcd_draw_fcircle(x + r, y + h - r, r, c);
    lcd_draw_fcircle(x + w - r, y + h - r, r, c);
    lcd_draw_fcircle(x + w - r, y + r, r, c);
  }
  else
    lcd_draw_frect(x, y, w, h, c);
}

//画空心园
void lcd_draw_circle(u16 x, u16 y, u16 r, u32 c)
{
  s32 D;    /* Decision Variable */
  u32 CurX; /* Current X Value */
  u32 CurY; /* Current Y Value */
  D = 3 - (r << 1);
  CurX = 0;
  CurY = r;
  D = 3 - (r << 1);
  CurX = 0;
  CurY = r;
  while (CurX <= CurY)
  {
    lcd_draw_point(x + CurX, y + CurY, c);
    lcd_draw_point(x + CurX, y - CurY, c);
    lcd_draw_point(x - CurX, y + CurY, c);
    lcd_draw_point(x - CurX, y - CurY, c);
    lcd_draw_point(x + CurY, y + CurX, c);
    lcd_draw_point(x + CurY, y - CurX, c);
    lcd_draw_point(x - CurY, y + CurX, c);
    lcd_draw_point(x - CurY, y - CurX, c);
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
void lcd_draw_fcircle(u16 x0, u16 y0, u16 r, u32 c)
{
  u32 i;
  u32 imax = ((u32)r * 707) / 1000 + 1;
  u32 sqmax = (u32)r * (u32)r + (u32)r / 2;
  u32 x = r;
  lcd_draw_hline(x0 - r, y0, 2 * r, c);
  for (i = 1; i <= imax; i++)
  {
    if ((i * i + x * x) > sqmax)
    {
      // draw lines from outside
      if (x > imax)
      {
        lcd_draw_hline(x0 - i + 1, y0 + x, 2 * (i - 1), c);
        lcd_draw_hline(x0 - i + 1, y0 - x, 2 * (i - 1), c);
      }
      x--;
    }
    // draw lines from inside (center)
    lcd_draw_hline(x0 - x, y0 + i, 2 * x, c);
    lcd_draw_hline(x0 - x, y0 - i, 2 * x, c);
  }
}
void lgui_draw_arc(u16 x0, u16 y0, u16 r, u16 s, u32 c)
{
  u32 i;
  u32 imax = ((u32)r * 707) / 1000 + 1;
  u32 sqmax = (u32)r * (u32)r + (u32)r / 2;
  u32 x = r;
  lcd_draw_hline(x0 - r, y0, 2 * r, c);
  for (i = 1; i <= imax; i++)
  {
    if ((i * i + x * x) > sqmax)
    {
      // draw lines from outside
      if (x > imax)
      {
        lcd_draw_hline(x0 - i + 1, y0 + x, 2 * (i - 1), c);
        lcd_draw_hline(x0 - i + 1, y0 - x, 2 * (i - 1), c);
      }
      x--;
    }
    // draw lines from inside (center)
    lcd_draw_hline(x0 - x, y0 + i, 2 * x, c);
    lcd_draw_hline(x0 - x, y0 - i, 2 * x, c);
  }
}
/**
 *椭圆
 */
void lgui_draw_ellipse(u16 x, u16 y, u16 a, u16 b, u32 color)
{
  u16 wx, wy;
  u16 thresh;
  u16 asq = a * a;
  u16 bsq = b * b;
  u16 xa, ya;

  lcd_draw_point(x, y + b, color);
  lcd_draw_point(x, y - b, color);

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

    lcd_draw_point(x + wx, y - wy, color);
    lcd_draw_point(x - wx, y - wy, color);
    lcd_draw_point(x + wx, y + wy, color);
    lcd_draw_point(x - wx, y + wy, color);
  }

  lcd_draw_point(x + a, y, color);
  lcd_draw_point(x - a, y, color);

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

    lcd_draw_point(x + wx, y - wy, color);
    lcd_draw_point(x - wx, y - wy, color);
    lcd_draw_point(x + wx, y + wy, color);
    lcd_draw_point(x - wx, y + wy, color);
  }
}

/////////////////////////////////以上tft//////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#endif