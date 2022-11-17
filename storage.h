
#ifndef _STORAGE_H_
#define _STORAGE_H_

#include <stdint.h>
#include "compiler_abstraction.h"

#define STORAGE_PAGE_SIZE (512)
#define STORAGE_PAGE_COUNT (2)
#define STORAGE_START_ADDR 0xE000
#define STORAGE_VALID_KEY 0xAA55
#define STORAGE_VALID_KEYH 0xAA
#define STORAGE_VALID_KEYL 0x55

void storage_write(void* save);
void storage_init(uint8_t size, void* save);

#endif
