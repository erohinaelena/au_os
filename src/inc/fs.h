#ifndef __FS_H__
#define __FS_H__
#include <stdint.h>
#include "spinlock.h"


struct file
{
	char name[256];
	struct file* next;
	struct file* child;
	uint32_t size;
	uint32_t size_pow;
	char* data;
	uint32_t offset;
	int is_dir;
	struct spinlock lock;
};

void init_fs();
struct file* open(char *path);
void close(struct file*);
void read(struct file* file, uint32_t offset, uint32_t size, char* buffer);
void write(struct file* file, uint32_t offset, uint32_t size, char* buffer);
void mkdir(char *path);
void touch(char *path);

struct file_list {
	char** list;
	uint32_t count;
};

struct file_list* readdir(char *path);
void delete_file_list(struct file_list* list);

#endif /* __FS_H__ */