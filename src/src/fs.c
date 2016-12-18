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
char* get_last_path_part(char* c) {
	char* pos = c;
	size_t i=0;
	while(i < strlen(c)) {
		if (c[i] == '/') pos = c + i + 1;
		i++;
	}
	return pos;
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

int strcmp_begin(char* name1, char* name2){
	for(size_t i = 0; i < strlen(name1); i++){
		if (name1[i] != name2[i]) return 0;
		i++;
	}
	return 1;
}
int has_dir (char* name){
	for (size_t i = 1; i < strlen(name)-1; i++) {
		if (name[i] == '/') return 1;
	}
	return 0;
}

struct file* find_parent_dir_of_file(char* path) {
	char* start = path;
	char* end = get_path_part(start+1);
	struct file* cur = root->child;
	struct file* parent = root;
	struct file* prev_neighbour = 0;
	spin_lock(&cur->lock);
	while (start != end) {
		if (cur == 0) {
			break;
		}
		if (strcmp_begin(cur->name, start)>0) {
			if (strlen(cur->name) == strlen(start)) {
				break;
			}
			parent = cur;
			cur = cur->child;
			if (cur) spin_lock(&cur->lock);
			spin_unlock(&parent->lock);
			char* next_end = get_path_part(end+1);
			start = end;
			end = next_end;
		}
		else {
			prev_neighbour = cur;
			cur = cur->next;
			if (cur) spin_lock(&cur->lock);
			spin_unlock(&prev_neighbour->lock);
		}
	}
	if (cur) spin_unlock(&cur->lock);
	if (has_dir(start)) {
		return 0;
	}
	return parent;
}

struct file* find_file_in_dir(struct file* dir, char* name) {
	struct file* cur = dir->child;
	while (cur) {
		if (strcmp_begin(cur->name, name)>0 && strlen(cur->name) == strlen(name)) {
			spin_unlock(&dir->lock);
			return cur;
		} else {
			cur = cur->next;
		}
	}
	return 0;
}

struct file* mkfile(struct file* parent, char* name) {
	
	printf("make file %s\n", name);
	
	struct file* node = create_file();
	node->next = parent->child;
	parent->child = node;
	strcpy(name, name + strlen(name), node->name);

	if (node->name[strlen(node->name)] != '\0'){
		node->name[strlen(node->name)] = '\0';
	}
	return node;
}

struct file* open(char* path){
	struct file* parent = find_parent_dir_of_file(path);
	if (parent) {
		spin_lock(&parent->lock);
		char* file_name = get_last_path_part(path);
		struct file* opened = find_file_in_dir(parent, file_name);
		if (!opened) {
			printf("not found %s, now will be created\n", file_name);
			opened = mkfile(parent, file_name);
		}
		printf("%s opened\n", file_name);
		spin_lock(&opened->lock);
		spin_unlock(&parent->lock);

		return opened;
	} else {
		return 0;
	}
}

void close(struct file* file) {
	spin_unlock(&file->lock);
}

uint32_t read(struct file* file, uint32_t offset, uint32_t size, char* buffer){
	uint32_t cur = 0;
	while (offset + cur < file->size && cur < size) {
		buffer[cur] = file->data[offset + cur];
		cur++;
	}
	return cur;
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
	while (file->size_pow < size + offset) {
		resize(file);
	}
	uint32_t cur = 0;
	while (cur < size) {
		file->data[offset + cur] = buffer[cur];
		cur++;
	}

	file->size = file->size + offset + size;
}
void mkdir(char* path){
	struct file* parent = find_parent_dir_of_file(path);
	if (parent) {
		spin_lock(&parent->lock);
		char* file_name = get_last_path_part(path);
		struct file* f = find_file_in_dir(parent, file_name);
		if (f) printf("file %s is already exist\n", file_name);
		else f = mkfile(parent, file_name);
		f->is_dir = 1;
		spin_unlock(&parent->lock);

	}
}
void touch(char *path){
	struct file* parent = find_parent_dir_of_file(path);
	if (parent) {
		spin_lock(&parent->lock);

		char* file_name = get_last_path_part(path);
		struct file* f = find_file_in_dir(parent, file_name);
		if (f) printf("file %s is already exist\n", file_name);
		else f = mkfile(parent, file_name);
		spin_unlock(&parent->lock);
	}
}

void list_resize(struct file_list* list) {
	char** new_list = mem_alloc(sizeof(char*)*list->count*2);
	for (uint32_t i; i < list->count; i++){
		new_list[i] = list->list[i];
	}
	mem_free(list->list);
	list->list = new_list;
}

struct file_list* readdir (char* path){
	struct file* parent = find_parent_dir_of_file(path);
	if (!parent) return 0;

	char* file_name = get_last_path_part(path);
	struct file* cur = find_file_in_dir(parent, file_name);
	if (!cur) return 0;

	struct file_list* list = mem_alloc(sizeof(struct file_list));
	uint32_t size = 256;
	list->list = mem_alloc(sizeof(char*)*size);

	printf("readdir\n");
	cur = cur->child;
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

