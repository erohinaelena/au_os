#include "fs.h"

#include "string.h"
#include "alloc.h"
#include "print.h"

static struct file* root;



struct file* create_file()
{
	struct file* file = mem_alloc(sizeof(struct file));

	file->name[0]='\0';
	file->is_dir = 0;
	file->next = 0;
	file->child = 0;
	spin_setup(&file->lock);
	file->size = 0;
	file->size_pow = 1;
	file->data = 0;

	return file;
}

void init_fs(){
	root = create_file();
	root->is_dir = 1;
}

char* get_path_part(char* c) {
	if (*c == '\0') {
		return c;
	}
	while (*c != '\0' && *c != '/') {
		c++;
	}
	return c;
}

void strcpy(char* start, char* end, char* dest){
	size_t i=0;
	if(start[0]=='/') start++;
	while (start + i <= end){
		dest[i] = start[i];
		i++;
	}
	if(*end != '\0') dest[i] = '\0';
}
int cmp(char* name, char* start, char* end) {
	while (*name != 0 && start != end) {
		if (*name != *start) {
			return 0;
		}
		name++;
		start++;
	}
	if (*name == '\0' && start == end) return 1;
	return 0;
}

int strcmp_begin(char* name1, char* name2){
	printf("strcmp\n");

	for(size_t i = 0; i < strlen(name1)-1; i++){
		if (name1[i] != name2[i]) return 0;
		i++;
	}
	return 1;
}

struct file* mkfile(struct file* file, char* name) {
	spin_lock(&file->lock);
	printf("make file\n");
	printf(name);
	printf("\n");
	if (cmp(file->name, name, name + strlen(name))){
		return file;
	}
	struct file* node = create_file();
	node->next = file->child;
	file->child = node;
	char* end = get_path_part(name+1);
	strcpy(name, end, node->name);

	if (node->name[strlen(node->name)-1] == '/'){
		node->name[strlen(node->name)-1] = '\0';
	}

	if (*end == '\0') {
		node->is_dir = 0;
		spin_unlock(&file->lock);
		return node;
	} else {
		node->is_dir = 1;
	}
	spin_unlock(&file->lock);
	return mkfile(node, end);
}

struct file* open(char* name){
	char* start = name;
	char* end = get_path_part(start+1);
	struct file* cur = root->child;
	struct file* prev = root;
	while (start != end) {
		if (cur == 0) {
			break;
		}
		if (cmp(cur->name, start, end)) {
			prev = cur;
			cur = cur->child;
			char* next_end = get_path_part(end+1);
			start = end;
			end = next_end;
		}
		else {
			cur = cur->next;
		}
	}
	if (cur == 0 || cur->is_dir == 1) {
		cur = mkfile(prev, start);
	}
	spin_lock(&cur->lock);
	return cur;
}
void close(struct file* file) {
	spin_unlock(&file->lock);
}

void read(struct file* file, uint32_t offset, uint32_t size, char* buffer){
	uint32_t cur = 0;
	file->offset = offset;
	while (file->offset < file->size && cur < size) {
		buffer[file->offset] = file->data[offset + cur];
		file->offset++;
		cur++;
	}
}
void resize(struct file* file) {
	printf("resize\n");
	char* data = mem_alloc(file->size_pow * 2);
	for (uint32_t i = 0; i < file->size; i++) {
		data[i] = file->data[i];
	}
	mem_free(file->data);
	file->data = data;
	file->size_pow *= 2;
}
void write(struct file* file, uint32_t offset, uint32_t size, char* buffer){
	while (file->size_pow - file->offset < size - offset) {
		resize(file);
	}
	uint32_t cur = 0;
	while (cur < size) {
		file->data[file->offset] = buffer[cur + offset];
		cur++;
		file->offset++;
	}

	file->size = file->offset;
}
void mkdir(char* name){
	mkfile(root, name)->is_dir = 1;
}
void touch(char *name){
	mkfile(root, name);
}

void list_resize(struct file_list* list) {
	char** new_list = mem_alloc(sizeof(char*)*list->count*2);
	for (uint32_t i; i < list->count; i++){
		new_list[i] = list->list[i];
	}
	mem_free(list->list);
	list->list = new_list;
}

struct file_list* readdir (char* name){
	struct file_list* list = mem_alloc(sizeof(struct file_list));
	uint32_t size = 256;
	list->list = mem_alloc(sizeof(char*)*size);
	char* start = name;
	char* end = get_path_part(start+1);
	struct file* cur = root->child;
	while (start != end) {
		printf("start c\n");
		if (cur == 0) {
			break;
		}
		if (strcmp_begin(cur->name, start)>0) {
			if (strlen(cur->name) == strlen(start)) break;
			char* next_end = get_path_part(end+1);
			start = end;
			end = next_end;
		} else {
			cur = cur->next;	
		}
	}

	cur = cur->child;
	printf("readdir\n");

	list->count = 0;

	if (cur) {
		while(cur->next){
			list->list[list->count] = cur->name;
			cur = cur->next;
			list->count++;
			if (list->count == size) {
				list_resize(list);
				size*=2;
			}
		}
	}
	list->list[list->count] = cur->name;
	list->count++;
	return list;
}

void delete_file_list(struct file_list* list) {
	mem_free(list->list);
	mem_free(list);
}

