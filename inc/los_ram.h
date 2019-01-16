#ifndef __LP_RAM_H__
#define __LP_RAM_H__

#include "stdint.h"
void *lpram_malloc(uint32_t size);
void lpram_free(void *ram);
void *lpram_realloc(void *ram, uint32_t size);
void *lpram_calloc(uint32_t n, uint32_t size);
#endif
