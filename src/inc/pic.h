#ifndef __PIC_H__
#define __PIC_H__

#include <stdint.h>

#define MASTER_COMMAND 0x20
#define MASTER_DATA 0x21

#define SLAVE_COMMAND 0xA0
#define SLAVE_DATA 0xA1

void init_pic();

void send_nsEOI(uint64_t int_num);

void set_interrupt_mask(uint16_t mask);

#endif /*__DESC_H__*/
