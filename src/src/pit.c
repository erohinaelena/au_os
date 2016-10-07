#include <pit.h>
#include <pic.h>
#include <ioport.h>
#include <desc.h>

void init_pit(){
	uint16_t init = 100;

	out8(COMMAND_PIT, 0x14);
	out8(DATA_PIT, init);
	out8(DATA_PIT, init >> 8);
	set_interrupt_mask(0xfe);
}
