#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  register int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite for the %dth time!", j);
    j ++;
    _yield();
  }
}

void init_proc() {
	context_kload(&pcb[0], (void *)hello_fun);
	context_uload(&pcb[2], "/bin/dummy");

  switch_boot_pcb();

  Log("Initializing processes...");

  // load program
	current = &pcb[0];
  Log("Jump to entry = %x", (void *)hello_fun);
	hello_fun(NULL);
	//naive_uload(NULL, "/bin/dummy");

}

_Context* schedule(_Context *prev) {
	/* save current context */
	current->cp = prev;
	assert(prev->cs == 8);
	/* switch to destination */
	current = (current == &pcb[0] ? &pcb[2] : &pcb[0]);
	//current = &pcb[0];
	//assert(current->cp->as->ptr);
	return current->cp;
}
