#include "common.h"
#include "syscall.h"
#include <klib.h>

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  switch (a[0]) {
		case SYS_exit: 
								Log("SYS_exit"); _halt(0);return 0;
		case SYS_yield: 
								Log("SYS_yield");_yield(); return 0;
		case SYS_write:
								Log("SYS_write");
								assert(c->GPR2 == 1 || c->GPR2 == 2);
								int i;
								for (i = 0; i < c->GPR4; ++i)
									_putc(*((char*)c->GPR3 + i));
								c->GPRx = i;
								return 0;
		case SYS_brk:
								c->GPRx = 0;
								/* TODO: implementation */
								return 0;

    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
