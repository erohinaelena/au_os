#include <ioport.h>
#include <desc.h>
#include <serial.h>

void serial_init(){
	out8(SERIAL_PORT + 1, 0x00);
	out8(SERIAL_PORT + 3, 0x80);
	out8(SERIAL_PORT + 0, 0x01); 
	out8(SERIAL_PORT + 1, 0x00); 
	out8(SERIAL_PORT + 3, 0x03);
}

void printStr(char * str) {
	int i = 0;	
	while (str[i]!='\0') {
	if(in8(SERIAL_PORT + 5) & 32) {
		out8(SERIAL_PORT + 0, str[i]);
		i++;		
	};	
	}
	
}

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
