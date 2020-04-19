#include "fs.h"
Finfo file_table[] = {
  {"stdin", 0, 0, 0, invalid_read, invalid_write},
  {"stdout", 0, 0, 0, invalid_read, serial_write},
  {"stderr", 0, 0, 0, invalid_read, serial_write},
  {"/dev/events", 0, 0, 0, events_read, invalid_write},
	{"/dev/fb", 0, 0, 0, invalid_read, fb_write},
	{"/dev/fbsync", 0, 0, 0, invalid_read, fbsync_write},
	{"/dev/tty",0, 0, 0, invalid_read, serial_write},
	{"/proc/dispinfo", 0 ,0, 0, dispinfo_read, invalid_write},
#include "files.h"
};


#define NR_FILES (sizeof(file_table)/sizeof(file_table[0]))

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}


extern size_t ramdisk_write(const void*, size_t, size_t);

void init_fs() {
	Finfo *cur = &file_table[fs_open("/dev/fb", 0, 0)];
	cur->size = screen_size() * sizeof(int32_t);
	file_table[fs_open("/proc/dispinfo", 0, 0)].size = 128;
  // TODO: initialize the size of /dev/fb
}

int fs_open(const char *pathname, int flags, int mode) {
	/* flags & mode are ignored temporarily */
	Log("pathname = %s", pathname);
	int i;	
	for (i = 0; i < NR_FILES; ++i) 
		if (!strcmp(pathname, file_table[i].name)){
			file_table[i].open_offset = 0;	
			return i;
		}
	panic("invalid pathname:%s!", pathname);
	assert(0);
	return -1;
}

size_t fs_read(int fd, void *buf, size_t len) {
	assert(fd > 2 && fd < NR_FILES);
	Finfo *cur = &file_table[fd];
	intptr_t sp = cur->disk_offset + cur->open_offset;
	assert(sp>= cur->disk_offset && sp < cur->disk_offset + cur->size);
	/* reading beyond boudary */
	if (len > cur->size - cur->open_offset)
		len = cur->size - cur->open_offset;
	ramdisk_read(buf, sp, len);	
	cur->open_offset += len;
	return len;
}	
size_t fs_write(int fd, const void *buf, size_t len) {
	assert(fd > 2 && fd < NR_FILES);
	Finfo *cur = &file_table[fd];
	intptr_t sp = cur->disk_offset + cur->open_offset;
	assert(sp>= cur->disk_offset && sp < cur->disk_offset + cur->size);
	/* writing beyond boundary */
	if (len > cur->size - cur->open_offset)
		len = cur->size - cur->open_offset;
	ramdisk_write(buf, sp, len);
	cur->open_offset += len;
	return len;
}
size_t fs_lseek(int fd, size_t offset, int whence) {
	assert(fd > 2 && fd < NR_FILES);
	Finfo *cur = &file_table[fd];
	size_t sp;
	switch (whence) {
		case SEEK_SET:sp = 0;break;
		case SEEK_CUR:sp = cur->open_offset;break;
		case SEEK_END:sp = cur->size;break;
		default : assert(0);
	}
	cur->open_offset = sp + offset;	
	return cur->open_offset;
}
int fs_close(int fd){
	return 0;
}
