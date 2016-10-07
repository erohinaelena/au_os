#include <desc.h>
#include <memory.h>
#include <desc.h>
#include <ioport.h>

struct idt_item {
	uint16_t offset1;
	uint16_t segment_selector;
	uint16_t type;
	uint16_t offset2;
	uint32_t offset3;
	uint32_t zeros;
} __attribute__((packed));


struct idt_item idt[IDT_SIZE];
struct desc_table_ptr ptr_idt;
extern uint64_t table[];

void init_IDT(){
	
	ptr_idt.size = IDT_SIZE * sizeof(struct idt_item) - 1;
    	ptr_idt.addr = (uint64_t) idt; 
    	write_idtr(&ptr_idt); 

	for (int i = 0; i < IDT_SIZE; i++) {
		struct idt_item item;
		uint64_t offset = table[i];
		item.zeros = 0;
    		item.offset3 = offset >> 32;
    		item.offset2 = offset >> 16;
    		item.type = 0x8E00; 
    		item.segment_selector = KERNEL_CS;
    		item.offset1 = offset;
    		idt[i] = item;
    	}
	
}


