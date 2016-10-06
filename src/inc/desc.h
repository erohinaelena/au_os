#ifndef __DESC_H__
#define __DESC_H__

#include <stdint.h>

#define SERIAL_PORT 0x3f8
#define IDT_SIZE 33

struct desc_table_ptr {
	uint16_t size;
	uint64_t addr;
} __attribute__((packed));

void init_IDT();
void serial_init();
void printStr(char * str);

static inline void read_idtr(struct desc_table_ptr *ptr)
{
	__asm__ ("sidt %0" : "=m"(*ptr));
}

static inline void write_idtr(const struct desc_table_ptr *ptr)
{
	__asm__ ("lidt %0" : : "m"(*ptr));
}

static inline void read_gdtr(struct desc_table_ptr *ptr)
{
	__asm__ ("sgdt %0" : "=m"(*ptr));
}

static inline void write_gdtr(const struct desc_table_ptr *ptr)
{
	__asm__ ("lgdt %0" : : "m"(*ptr));
}

#endif /*__DESC_H__*/
