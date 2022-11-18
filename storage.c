
#include "storage.h"

__WEAK __INLINE void flash_write(uint8_t* from, uint32_t to, uint16_t length) {}
__WEAK __INLINE void flash_erase(uint32_t addr) {}

__align(STORAGE_PAGE_SIZE) static const uint8_t storage[STORAGE_PAGE_SIZE * STORAGE_PAGE_COUNT] __attribute__((at(STORAGE_START_ADDR)));

static uint8_t storage_unit_size = 0;
static uint16_t storage_last_usable = 0xFFFF;

int8_t storage_read_last(void* target) {
	if(storage_unit_size < 2) {
		return -1;
	}
	if(storage_unit_size > STORAGE_PAGE_SIZE) {
		return -2;
	}
	if((storage[storage_unit_size - 2] != STORAGE_VALID_KEYH) || (storage[storage_unit_size - 1] != STORAGE_VALID_KEYL)) {
		if((storage[storage_unit_size - 2] != 0xFF) || (storage[storage_unit_size - 1] != 0xFF)) {
			// Not init
			storage_last_usable = 0xFFFF;
			return -4;
		}
		storage_last_usable = 0;
		// No data
		return -3;
	}
	uint16_t p = 0;
	while(p < STORAGE_PAGE_SIZE * STORAGE_PAGE_COUNT - storage_unit_size) {
		if((storage[p + storage_unit_size - 2] != STORAGE_VALID_KEYH) || (storage[p + storage_unit_size - 1] != STORAGE_VALID_KEYL)) {
			break;
		}
		p += storage_unit_size;
	}
	storage_last_usable = p;
	p -= storage_unit_size;
	uint8_t* src = (uint8_t*)&(storage[p]);
	for(uint16_t i = 0; i < storage_unit_size - 2; i++) {
		*((uint8_t*)target) = *src++;
		(uint32_t) target += 1;
	}
	return 0;
}

void storage_write(void* save) {
	uint16_t write_addr = storage_last_usable;
	if(write_addr == 0xFFFF) {
		for(uint8_t i = 0; i < STORAGE_PAGE_COUNT; i++) {
			flash_erase(STORAGE_START_ADDR + i * STORAGE_PAGE_SIZE);
		}
		write_addr = 0;
	}

	uint8_t key[2] = { STORAGE_VALID_KEYH, STORAGE_VALID_KEYL };
	flash_write(save, (uint32_t) & (storage[write_addr]), storage_unit_size - 2);
	flash_write(key, (uint32_t) & (storage[write_addr]) + storage_unit_size - 2, 2);
	if(write_addr < STORAGE_PAGE_SIZE * STORAGE_PAGE_COUNT - storage_unit_size) {
		storage_last_usable += storage_unit_size;
	} else {
		storage_last_usable = 0xFFFF;
	}
}

void storage_init(uint8_t size, void* save) {
	storage_unit_size = size + 2;
	if(storage_unit_size & 0x03) {
		storage_unit_size += 4 - (storage_unit_size & 0x03);
	}
	storage_read_last(save);
}
