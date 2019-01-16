/*----------------------------------------------------------------------------/
 /  los -  system module  R0.1
 /-----------------------------------------------------------------------------/
 /
 / Copyright (c) 2014-2017 LP电子,All Rights Reserved.
 /
 / 未经授权，禁止商用。
 /----------------------------------------------------------------------------*/

#include "los_ram.h"
#include "stdio.h"
void *lpram_malloc(uint32_t size)
{
    return (void *)malloc(size);
}
void lpram_free(void *ram)
{
    free(ram);
}
void *lpram_realloc(void *ram, uint32_t size)
{
    return realloc(ram, size);
}
void *lpram_calloc(uint32_t n, uint32_t size)
{
    return (void *)calloc(n, size);
}
