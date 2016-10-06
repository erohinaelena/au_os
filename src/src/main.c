static void qemu_gdb_hang(void)
{
#ifdef DEBUG
	static volatile int wait = 1;

	while (wait);
#endif
}

#include <desc.h>
#include <ints.h>
#include <ioport.h>
#include <pic.h>
#include <pit.h>

void main(void)
{
	qemu_gdb_hang();

	serial_init();
	char* hello = "hello, world\n";
	printStr(hello);
	
	init_IDT();
	init_pic();

	enable_ints();
	init_pit();

	
	while (1){};
}
