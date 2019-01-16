/*----------------------------------------------------------------------------/
 /  los -  system module  R0.1
 /-----------------------------------------------------------------------------/
 /
 / Copyright (c) 2014-2017 LP电子,All Rights Reserved.
 /
 / 未经授权，禁止商用。
 /----------------------------------------------------------------------------*/
#include "lpram.h"
#include "lp_log.h"
#include "FreeRTOS.h"
void *lpram_malloc(uint32_t size)
{
    void *v = (void *)pvPortMalloc(size);
    return v;
}
void lpram_free(void *ram)
{
    vPortFree(ram);
}
void *lpram_realloc(void *ram, uint32_t size)
{
    void *buf;
    buf = (void *)pvPortMalloc(size);
    if (buf)
    {
        memcpy(buf, ram, size);
    }
    else
        return 0;
}
void *lpram_calloc(uint32_t n, uint32_t size)
{
    void *v = (void *)pvPortMalloc(size * n);
    if (v )
        memset(v, 0, size * n);
    return v;
}