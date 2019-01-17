/*----------------------------------------------------------------------------/
 /  los -  system module  R0.1
 /-----------------------------------------------------------------------------/
 /
 / Copyright (c) 2014-2017 LP电子,All Rights Reserved.Author's mailbox:lgtbp@126.com.
 /
 / 未经授权，禁止商用。Commercial use is prohibited without authorization.
 /----------------------------------------------------------------------------*/
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
