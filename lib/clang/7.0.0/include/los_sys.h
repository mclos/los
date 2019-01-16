/**
 * @file los_sys.h
 * @brief los系统api
 * @details
 * @mainpage
 * @author lp
 * @email lgtbp@126.com
 * @version 1
 * @license Copyright © 2018, lp. All rights reserved.
 *			This project (los) shall not be used for profit by any unit or individual without permission;
 *		if it is used for non-profit use such as personal use and communication,
 *      the author shall be specified and the source shall be marked.
 *         The copyright owner is not liable for any direct, indirect, incidental, special, disciplinary or
 *      consequential damages (including but not limited to the purchase of substitute goods or services;
 *      loss of use, data or profits; or commercial interruption) and then causes any liability theory,
 *      whether Contract, strict liability, or infringement (including negligence or otherwise),
 *      use the Software in any way, even if it is informed of the possibility of such damage.
 */

#ifndef _LOS_SYS_API_H_
#define _LOS_SYS_API_H_

#include"stdint.h"

#define  semaphore2_t   uint8_t    ///< 2值信号量
#define  semaphoreflag_t   uint8_t  ///< 信号量
typedef void (*thread_func_t)(void); ///< 线程函数类型
typedef void (*msg_func_t)(uint32_t id,uint32_t hram,uint32_t lram); ///< 线程函数类型
/**
 * @brief 线程的类型 los thread type
 */
enum LOS_THREAD_TYPE {
	LOS_THREAD_UI = 0, ///< ui线程
	LOS_THREAD_TIME,  ///< time线程
	LOS_THREAD_SERVER, ///< 服务线程
};

/**
 * @brief 系统等待事件
 * @return no
 */
void los_wait_event(void);

/**
 * @brief 检查事件发生
 *
 * @return
 * @retval 0:没事件
 * @retval other 有事件
 */
int los_set_msg(msg_func_t *msg);

/**
 * @brief 线程延时
 *
 * @param ms 延时ms毫秒
 * @return no
 */
void los_delay(uint32_t ms);
/**
 * @brief los系统打印
 *
 * @param fmt 打印格式
 * @param ... 打印数据
 * @return no
 */
void los_printf(const char * fmt, ...);
/**
 * @brief 获取当前线程的栈使用大小
 *
 * @return 返回大小
 */
uint32_t los_stack_use_size(void);
/**
 * @brief 创建一个线程
 *
 * @param id 线程id，从1开始
 * @param type 线程类型（参考LOS_THREAD_TYPE）
 * @param func 线程函数
 * @return
 * @retval 0:ok
 * @retval other err
 */
uint32_t los_thread_new(uint16_t id, uint8_t type, thread_func_t * func);
/**
 * @brief 启动一个线程
 *
 * @param id 线程id
 * @return
 * @retval 0:ok
 * @retval other err
 */
uint32_t los_thread_start(uint16_t id);
/**
 * @brief 停止一个线程
 *
 * @param id 线程id
 * @return
 * @retval 0:ok
 * @retval other err
 */
uint32_t los_thread_stop(uint16_t id);
/**
 * @brief 删除一个线程
 *
 * @param id 线程id
 * @return
 * @retval 0:ok
 * @retval other err
 */
uint32_t los_thread_del(uint16_t id);
/**
 * @brief 结束当前定时器
 *
 * @return
 * @retval 0:ok
 * @retval other err
 */
uint32_t los_time_end(void);
/**
 * @brief 设置一个线程的调用间隔
 *
 * @param id 线程id，从1开始
 * @param ms 延时ms毫秒
 * @return
 * @retval 0:ok
 * @retval other err
 */
uint32_t los_thread_set_time(uint16_t id, uint32_t ms);
/**
 * @brief 获取一个2值信号量，直至获取成功。
 *
 * @param semaphore2 2值信号量
 * @return
 * @retval 0:ok
 * @retval other err
 */
uint32_t los_semaphore2_get(semaphore2_t *semaphore2);
/**
 * @brief 发送一个2值信号量
 *
 * @param semaphore2 2值信号量
 * @return
 * @retval 0:ok
 * @retval other err
 */
uint32_t los_semaphore2_send(semaphore2_t *semaphore2);
/**
 * @brief 获取一个信号量，直至获取成功。
 *
 * @param semaphore2 2值信号量
 * @return
 * @retval 0:ok
 * @retval other err
 */
uint32_t los_semaphoreflag_get(semaphoreflag_t *semaphore2);
/**
 * @brief 释放一个信号量
 *
 * @param semaphore2 2值信号量
 * @return
 * @retval 0:ok
 * @retval other err
 */
uint32_t los_semaphoreflag_free(semaphoreflag_t *semaphore2);
/**
 * @brief 开启或关闭中断
 *
 * @param eanble 1开始，0关闭
 * @return
 * @retval 0:ok
 * @retval other err
 */
uint32_t los_iqr_eanble(uint8_t eanble);
/**
 * @brief 退出los
 *
 * @return
 * @retval 0:ok
 * @retval other err
 */
uint32_t los_exit(void);
/**
 * @brief 获取los版本号
 *
 * @return
 * @retval 0:ok
 * @retval other err
 */
uint16_t los_version(void);
/**
 * @brief 调用一个los app
 *
 * @param path_name app名字
 * @param arg  参数
 * @return
 * @retval 0:ok
 * @retval other err
 */
uint32_t los_exe_start(char *path_name, char *arg);
/**
 * @brief 暫停一个los app
 *
 * @param path_name app名字
 * @return
 * @retval 0:ok
 * @retval other err
 */
uint32_t los_exe_stop(char *path_name);
/**
 * @brief 退出一个los app
 *
 * @param path_name app名字
 * @return
 * @retval 0:ok
 * @retval other err
 */
uint32_t los_exe_exit(char *path_name);
/**
 * @brief los app的arg参数
 *
 * @return
 * @retval 0:ok
 * @retval other err
 */
uint8_t los_arg_c(void);
/**
 * @brief los app的arg参数
 *
 * @param arg_c arg参数的序号
 * @return
 * @retval argc
 * @retval other err
 */
char *los_arg_v(uint8_t arg_c);
/**
 * @brief los系统,自定义函数
 *
 * @param id 打印格式
 * @param ... 打印数据
 * @return no
 */
void los_function(int id, ...);
/**
 * @brief 整個los延时
 *
 * @param ms 延时ms毫秒
 * @return no
 */
void los_delay_ms(uint32_t ms);

/**
 * @brief play aduio
 *
 * @param name file name
 * @param cb  callbcak
 * @param type paly type
 * @return
 * @retval 0:ok
 * @retval other err
 */
int play_audio(char *name, void *cb, uint32_t type);
#endif
