#include <ioport.h>
#include <desc.h>

void printStr(char * str) {
	int i = 0;	
	while (str[i]!='\0') {
	if(in8(SERIAL_PORT + 5) & 32) {
		out8(SERIAL_PORT + 0, str[i]);
		i++;		
	};	
	}
	
}
