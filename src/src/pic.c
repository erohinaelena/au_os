#include <ioport.h>
#include <pic.h>

void init_pic() {
	out8(MASTER_COMMAND, 17);
	out8(SLAVE_COMMAND, 17);

	out8(MASTER_DATA, 32);
	out8(SLAVE_DATA, 40);

	out8(MASTER_DATA, 4);
	out8(SLAVE_DATA, 2);

	out8(MASTER_DATA, 1);
	out8(SLAVE_DATA, 1);
}

void send_nsEOI(uint64_t int_num) {
	if (int_num >= 32) {
		out8(MASTER_COMMAND, 32);
		out8(SLAVE_COMMAND, 32);
	}
}

void set_interrupt_mask(uint8_t mask) {
	out8(MASTER_DATA, mask);
	out8(SLAVE_DATA, mask);
}
