#include "common.h"
#include "syscall.h"
#include "fs.h"
#include <klib.h>

extern Finfo file_table[];

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  switch (a[0]) {
		case SYS_exit: 
								_halt(0);return 0;
		case SYS_yield: 
								_yield(); return 0;
		case SYS_open:
								c->GPRx = fs_open((const char *)c->GPR2, 0, 0);
								return 0;
		case SYS_read:
								if (file_table[c->GPR2].read)
									c->GPRx = file_table[c->GPR2].read((char *)c->GPR3, file_table[c->GPR2].open_offset, c->GPR4);
								else
									c->GPRx = fs_read(c->GPR2, (char *)c->GPR3, c->GPR4);
								return 0;
		case SYS_write:
								if (file_table[c->GPR2].write) {
									c->GPRx = file_table[c->GPR2].write((const char *)c->GPR3, file_table[c->GPR2].open_offset, c->GPR4);
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

    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
