/*
 * usart.c
 *
 *  Created on: 2018��9��4��
 *      Author: PC
 */

#include "stdint.h"

typedef struct   //共16字节
{
  //这里一个任务的指针。
  uint8_t id;      //串口序号
  uint8_t stop;    //停止位
  uint8_t num;     //数据位
  uint8_t ver;     //检验位
  uint32_t prot;   //串口速度
  uint16_t cancel; //缓冲大小
} los_uart_t;
#define USART_NUM 1               //系统usart也算
#define USART_NUM_S USART_NUM + 1 //系统usart也算
typedef struct          //共16字节
{
  // uint8_t pf; //这是los中的printf，默认映射到系统，串口，也可以让uart映射
  //uint8_t id[USART_NUM]; //这是那个窗口的-
  //这里一个任务的指针。
  uint8_t *buf[USART_NUM]; //串口缓存
  uint8_t flg[USART_NUM];  //接受结束
  //0是在接受数据
  //1是已经接受到了
  //2是让系统重新开始，接受
  uint16_t len[USART_NUM];   //缓冲大小
  uint16_t rxlen[USART_NUM]; //接受大小
  uint32_t cb[USART_NUM];    //回调地址
  uint32_t los[USART_NUM];   //记录是谁的los-推出就自动推出这个uart
} los_port_t;

  void usart_init_los(void *los);

  void usart_disinit_los(void *los);

  void usart_send_byte_los(void *los);

  void usart_send_bytes_los(void *los);

  void usart_read_byte_los(void *los);

