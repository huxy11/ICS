#include "common.h"
#include "syscall.h"
#include "fs.h"
#include "proc.h"
#include <klib.h>

extern Finfo file_table[];

#define DEV file_table[c->GPR2]

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  switch (a[0]) {
		case SYS_exit: 
								naive_uload(NULL, "/bin/init");	
								//_halt(0);
								return 0;
		case SYS_yield:
								return schedule(c);
		case SYS_open:
								c->GPRx = fs_open((const char *)c->GPR2, 0, 0);
								return 0;
		case SYS_read:
								//Log("read @ %s", DEV.name);
								if (DEV.read)	{
									c->GPRx = DEV.read((char *)c->GPR3, DEV.open_offset, c->GPR4);
									DEV.open_offset += c->GPRx;
									if (DEV.size && DEV.open_offset > DEV.size) {
										Log("name = %s", DEV.name);
										Log("offset = %d size = %d", DEV.open_offset, DEV.size);
										assert(0);
									}
								}
								else{
									c->GPRx = fs_read(c->GPR2, (char *)c->GPR3, c->GPR4);
								}
								return 0;
		case SYS_write:
								if (DEV.write) {
									c->GPRx = DEV.write((const char *)c->GPR3, DEV.open_offset, c->GPR4);
									DEV.open_offset += c->GPRx;
									if (DEV.size && DEV.open_offset > DEV.size) {
										Log("name = %s", DEV.name);
										Log("offset = %d size = %d", DEV.open_offset, DEV.size);
										assert(0);
									}
								} else {
									c->GPRx = fs_write(c->GPR2, (const char *)c->GPR3, c->GPR4);
								}
								return 0;
		case SYS_close:
								c->GPRx = fs_close(c->GPR2);
								return 0;
		case SYS_lseek:
								c->GPRx = fs_lseek(c->GPR2, c->GPR3, c->GPR4);
								return 0;
		case SYS_brk:
								c->GPRx = 0;
								/* TODO: implementation */
								return 0;
		case SYS_execve:
								Log("name = %s", (char *)c->GPR2);
								naive_uload(NULL, (const char *)c->GPR2);
								c->GPRx = -1;
								return 0;

    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
#undef DEV
