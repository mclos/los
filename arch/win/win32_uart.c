/*----------------------------------------------------------------------------/
 /  los -  system module  R0.1
 /-----------------------------------------------------------------------------/
 /
 / Copyright (c) 2014-2017 LP电子,All Rights Reserved.
 /
 / 未经授权，禁止商用。
 /----------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <winbase.h>
#include <stdint.h>
static HANDLE los_comm;
static uint8_t open = 0; //设备状态 0.close 1 open
 
typedef void (*deal_rx_cmd_t)(uint8_t *buf, uint32_t len); //clang
 
static COMSTAT Comstat;
static OVERLAPPED OverLapped;
static char str_coom[8]; //com口
#define READ_LEN 1024 * 1024
static uint8_t *rxbuf;
static uint32_t rxbuf_len = 0;
static int SetupDCB(int rate_arg)
{
    DCB dcb;
    memset(&dcb, 0, sizeof(dcb));
    if (!GetCommState(los_comm, &dcb))
    { //获取当前DCB配置
        return 0;
    }
    dcb.DCBlength = sizeof(dcb);
    /* ---------- Serial Port Config ------- */
    dcb.BaudRate = rate_arg;
    dcb.Parity = NOPARITY;
    dcb.fParity = 0;
    dcb.StopBits = ONESTOPBIT;
    dcb.ByteSize = 8;
    dcb.fOutxCtsFlow = 0;
    dcb.fOutxDsrFlow = 0;
    dcb.fDtrControl = DTR_CONTROL_DISABLE;
    dcb.fDsrSensitivity = 0;
    dcb.fRtsControl = RTS_CONTROL_DISABLE;
    dcb.fOutX = 0;
    dcb.fInX = 0;
    dcb.fErrorChar = 0;
    dcb.fBinary = 1;
    dcb.fNull = 0;
    dcb.fAbortOnError = 0;
    dcb.wReserved = 0;
    dcb.XonLim = 2;
    dcb.XoffLim = 4;
    dcb.XonChar = 0x13;
    dcb.XoffChar = 0x19;
    dcb.EvtChar = 0;
    if (!SetCommState(los_comm, &dcb))
    {
        return 0;
    }
    else
        return 1;
}

static int SetupTimeout(DWORD ReadInterval, DWORD ReadTotalMultiplier,
                 DWORD ReadTotalConstant, DWORD WriteTotalMultiplier,
                 DWORD WriteTotalConstant)
{
    COMMTIMEOUTS timeouts;
    timeouts.ReadIntervalTimeout = ReadInterval;
    timeouts.ReadTotalTimeoutConstant = ReadTotalConstant;
    timeouts.ReadTotalTimeoutMultiplier = ReadTotalMultiplier;
    timeouts.WriteTotalTimeoutConstant = WriteTotalConstant;
    timeouts.WriteTotalTimeoutMultiplier = WriteTotalMultiplier;
    if (!SetCommTimeouts(los_comm, &timeouts))
    {
        return 0;
    }
    else
        return 1;
}

//串口初始化
int uart_comm_init(char *scom)
{
    strcpy(str_coom, scom);
    los_comm = CreateFile(scom, GENERIC_READ | GENERIC_WRITE, 0, 0,
                          OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);
    if (los_comm == INVALID_HANDLE_VALUE)
    {
        printf("comm open err!\n");
        return 1;
    }
    if (SetupDCB(115200) == 0)
    {
        printf("comm open err!\n");
        return 1;
    }
    if (SetupTimeout(0, 0, 0, 0, 0) == 0)
    {
        printf("comm open err!\n");
        return 1;
    }
    PurgeComm(los_comm,
              PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
    open = 1;
    printf("comm open ok\n");
    return 0;
}
static int uart_WriteChar(char *szWriteBuffer, DWORD dwSend)
{
    int bWrite = 1;
    int bResult = 1;
    DWORD BytesSent = 0;
    HANDLE hWriteEvent = NULL;
    ResetEvent(hWriteEvent);
    if (bWrite)
    {
        OverLapped.Offset = 0;
        OverLapped.OffsetHigh = 0;
        bResult = WriteFile(los_comm,      //通信设备句柄，CreateFile()返回值得到
                            szWriteBuffer, //指向写入数据缓冲区
                            dwSend,        //设置要写的字节数
                            &BytesSent,    //
                            &OverLapped);  //指向异步I/O数据
        if (!bResult)
        {
            DWORD dwError = GetLastError();
            switch (dwError)
            {
            case ERROR_IO_PENDING:
                BytesSent = 0;
                bWrite = FALSE;
                break;
            default:
                break;
            }
        }
    }
    if (!bWrite)
    {
        bWrite = TRUE;
        bResult = GetOverlappedResult(los_comm, &OverLapped, &BytesSent,
                                      TRUE);
        if (!bResult)
        {
            printf("GetOverlappedResults() in WriteFile()");
        }
    }
    if (BytesSent != dwSend)
    {
        printf("WARNING: WriteFile() error.. Bytes Sent:");
    }
    return TRUE;
}

//串口发送函数
int32_t uart_comm_send(char *p, int len)
{
    if (open == 0)
    {
        printf("err:comm is close\n");
        return -1;
    }
    uart_WriteChar(p, len);
    printf("comm send ok\n");
    return 0;
}

//串口读取函数

//关闭comm
int uart_comm_close()
{
    if (CloseHandle(los_comm))
    {
        open = 0;
        printf("close ok\n");
    }
    else
    {
        printf("close err\n");
    }
    return open;
}
static uint32_t time_tick = 0;
static uint32_t flgs = 0; //1表示有突变
static uint32_t ti = 0;
static int ReciveChar()
{
    int bResult = 1;
    DWORD dwError = 0;
    char buf[280];
    uint32_t rnum = 0;
    ti = 0;
    for (;;)
    {
        bResult = ClearCommError(los_comm, &dwError, &Comstat);
        if (Comstat.cbInQue == 0)
        {
            Sleep(10);  
            continue;
        }
        bResult = ReadFile(los_comm,     
                           buf,           
                           1,            
                           &rnum,        //
                           &OverLapped); //OVERLAPPED结构
         if (bResult)
        {
            flgs = 1;
            memcpy(&rxbuf[ti], buf, rnum);
            ti += rnum;  
        }
    }
    return 0;
}
DWORD WINAPI MyThreadFunction_uart(LPVOID lpParam)
{
    while (1)
    {
        if (open)
        {
            ReciveChar();
        }
        else
        {
            Sleep(100);
        }
    }
}
//接受信息
void  uart_rx(char *data,uint32_t len)
{

}

/**
 * uart注册的线程
 * */
void fun_uart(void *v)
{
    uint8_t ret = 0;
    while (1)
    {
        if (flgs)
        {
            time_tick++;
            if (time_tick > ti)
            {
                rxbuf[ti] = 0;
                uart_rx(rxbuf, ti);  
                time_tick = 0;
                flgs = 0;
                ti = 0;
            }
            Sleep(3);
        }
        else
            Sleep(50);
    }
    exit(1);
}
static HANDLE handle;
 
int win_init_thread_uart()
{
    DWORD ThreadID;
    HANDLE hThread;
    handle = GetStdHandle(STD_OUTPUT_HANDLE);
    hThread = CreateThread(NULL, 0, MyThreadFunction_uart, NULL, 0, &ThreadID);
    rxbuf = malloc(READ_LEN);
    rxbuf_len = 0;
    _beginthread(fun_uart, 1, NULL);
    return 0;
}
