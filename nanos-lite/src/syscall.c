#include "common.h"
#include "syscall.h"
#include <klib.h>

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
	printf("%a[0] = %#x\n", c->GPR1);

  switch (a[0]) {
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
