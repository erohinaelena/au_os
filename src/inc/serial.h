#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <stdint.h>
#define SERIAL_PORT 0x3f8

void serial_init();
void printStr(char * str);
void print_number(uint64_t number);

#endif /*__SERIAL_H__*/
