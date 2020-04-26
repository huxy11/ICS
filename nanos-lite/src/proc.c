#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
PCB pcb_boot = {};
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

static inline uint32_t get_cr3() {
	volatile uint32_t val;
	asm volatile ("movl %%cr3, %0" : "=r"(val));
	return val;
}

void init_proc() {
	context_kload(&pcb_boot, hello_fun);
	context_uload(&pcb[0], "/bin/init");


  switch_boot_pcb();

  Log("Initializing processes...");

  // load program
	void *entry = (void*)pcb_boot.cp->ip ;
  Log("Jump to entry = 0x%x", (uint32_t)entry);
	((void(*)())entry) ();
	//naive_uload(NULL, "/bin/dummy");

}

_Context* schedule(_Context *prev) {
	/* save current context */
	current->cp = prev;
	assert(prev->cs == 8);
	/* switch to destination */
	current = (current == &pcb_boot ? &pcb[0] : &pcb_boot);
	//current = &pcb[0];
	return current->cp;
}
