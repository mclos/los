/*----------------------------------------------------------------------------/
 /  los -  system module  R0.1
 /-----------------------------------------------------------------------------/
 /
 / Copyright (c) 2014-2017 LP电子,All Rights Reserved.Author's mailbox:lgtbp@126.com.
 /
 / 未经授权，禁止商用。Commercial use is prohibited without authorization.
 /----------------------------------------------------------------------------*/
#include "los.h"
//test数值是clang编译出来的，使用前请将clang路径添加到系统环境
//The test value is compiled by clang. Please add the clang path to the system environment before use.
//clang -c test.c
//losld -o test test.o  out .los file
/* 这是test.c内容
#include<los.h>
#define  set_callback(...)   los_call(0,##__VA_ARGS__)
#define  printf_num(...)  los_call(1,##__VA_ARGS__)
#define  get_num(...)   los_call(2,##__VA_ARGS__)
#define  do_cb(...)   los_call(3,##__VA_ARGS__)
void test_cb(unsigned char a){   //callback function
	unsigned char b=10;
	a++;
	a+=b;
	printf_num(a);           //los printf a num
}
int main(){
	int i=10;
	set_callback(test_cb); //set the call back function
	get_num(&i);           //get the var  for los vm
	printf_num(i);         //los printf a num
	do_cb();               //do callback function
	return 0;
}
*/
unsigned char test[135] = {
	0x6C, 0x6F, 0x73, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5F, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x01, 0x08, 0x3D, 0x08, 0x00, 0x3E, 0x30, 0x08, 
	0x3D, 0x08, 0x0A, 0x3E, 0x30, 0x04, 0x3D, 0x10, 0x2D, 0x00, 0x0D, 0x20, 0x0F, 0x00, 0x1D, 0x40, 
	0x08, 0x04, 0x0D, 0x20, 0x0F, 0x02, 0x1D, 0x3F, 0x30, 0x04, 0x0D, 0x20, 0x0F, 0x01, 0x1D, 0x0F, 
	0x03, 0x1D, 0x05, 0x00, 0x00, 0x01, 0x01, 0x3D, 0x08, 0x0A, 0x3E, 0x20, 0x01, 0x3F, 0x20, 0x05, 
	0x3D, 0x09, 0x01, 0x3A, 0x00, 0x08, 0x3E, 0x20, 0x05, 0x3F, 0x20, 0x01, 0x21, 0x10, 0x3F, 0x21, 
	0x05, 0x21, 0x11, 0x3A, 0x00, 0x01, 0x28, 0x41, 0x3E, 0x21, 0x05, 0x3F, 0x20, 0x05, 0x21, 0x10, 
	0x0D, 0x20, 0x0F, 0x01, 0x1D, 0x0A, 0x05
};
typedef uint32_t (*fun_os)(losc_t *);
uint32_t  addr; //los call back addr
uint32_t set_callback(losc_t *lp){
	addr=los_get_u32(lp,2);
	return 0;
}
uint32_t printf_num(losc_t *lp){
	printf("printf_num=%d\r\n", los_get_u8(lp,2));
	return 0;
}
uint32_t get_num(losc_t *lp){
	int *p=los_get_p(lp,2);
	p[0]=20;
	return 0;
}
uint32_t do_cb(losc_t *lp){
	los_arg_clear(lp);
	los_push_arg(lp,20);
	los_call_addr(lp,addr);
	return 0;
}
const fun_os los_sys_api[] = {
   set_callback,
   printf_num,
   get_num,
   do_cb,
};
uint32_t func(losc_t *lp){
  uint32_t id =los_get_u8(lp, 1);
  los_sys_api[id](lp);
}
int main(){
  int res;
  los_set_function(func); 
  res = los_app_first(test);
  printf("res:%d", res);
  getchar();
  return 0;
}
/** 执行输出:Execution output:
printf_num=20
printf_num=31
res:17
*/