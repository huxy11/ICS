#include "common.h"
#include "syscall.h"
#include "proc.h"
extern _Context* do_syscall(_Context*);

static _Context* do_event(_Event e, _Context* c) {
  switch (e.event) {
		case _EVENT_YIELD: 
			return schedule(c);
		case _EVENT_SYSCALL:
			return do_syscall(c);
		case _EVENT_IRQ_TIMER:
			_yield();
			return NULL;
    default: panic("Unhandled event ID = %d", e.event);
  }

  return NULL;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  _cte_init(do_event);
}
