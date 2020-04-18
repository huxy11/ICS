#ifndef __FS_H__
#define __FS_H__

#include "common.h"
size_t serial_write(const void *, size_t, size_t);
size_t fb_write(const void *, size_t, size_t);
size_t dispinfo_read(void *, size_t, size_t);
size_t fbsync_write(const void *, size_t, size_t);
size_t screen_size(void);

int fs_open(const char *, int, int);
size_t fs_read(int, void *, size_t);
size_t fs_write(int, const void *, size_t);
size_t fs_lseek(int, size_t, int);
int fs_close(int);

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);
size_t invalid_read(void *, size_t, size_t);
size_t invalid_write(const void *, size_t, size_t);

size_t events_read(void *, size_t, size_t);

typedef struct {
	  char *name;
		size_t size;
		size_t disk_offset;
		size_t open_offset;
		ReadFn read;
		WriteFn write;
} Finfo;


#ifndef SEEK_SET
enum {SEEK_SET, SEEK_CUR, SEEK_END};
#endif

#endif
