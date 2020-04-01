#include "common.h"
#include "syscall.h"

static _Context* do_event(_Event e, _Context* c) {
	Log("e.event = %d", e.event);
  switch (e.event) {
		case _EVENT_YIELD: Log("Yield!");break;
		case _EVENT_SYSCALL: switch (c->GPR1) {
													 case SYS_exit: Log("SYS_exit arg = %#x, %#x, %#x", c->GPR2, c->GPR3, c->GPR4); _halt(0);return 0;
													 case SYS_yield: Log("Syscall GPR1 = %d", c->GPR1);_yield(); return 0;
												 }
    default: panic("Unhandled event ID = %d", e.event);
  }

  return NULL;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  _cte_init(do_event);
}
