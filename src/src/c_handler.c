#include <desc.h>
#include <pic.h>

struct frame 
{
	uint64_t r15;
	uint64_t r14;
	uint64_t r13;
	uint64_t r12;
	uint64_t r11;
	uint64_t r10;
	uint64_t r9;
	uint64_t r8;
	uint64_t rsi;
	uint64_t rdi;
	uint64_t rbp;
	uint64_t rdx;
	uint64_t rcx;
	uint64_t rbx;
	uint64_t rax;
	uint64_t int_num;
	uint64_t error_code;
}__attribute__((packed));


void print_number(uint64_t number) {
	uint64_t number_part;
	int length = 0;
	char char_number[4];
	while(number > 0){
		number_part = number % 10;
		number = number / 10;
		char_number[length] = (char)(48 + number_part);
		length++;
	}
	for (int i = 0; i < length / 2; i++){
		char tmp = char_number[i];
		char_number[i] = char_number[length - i - 1];
		char_number[length - i - 1] = tmp;
	}
	
	char_number[length]='\0';
	printStr(char_number);

}

void c_handler(struct frame* regs) {
	printStr("\nint! ");
	print_number(regs -> int_num);
	send_nsEOI(regs -> int_num);
}
