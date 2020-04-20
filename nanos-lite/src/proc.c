#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;
_Context *cur_c = NULL;

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite for the %dth time!", j);
    j ++;
    _yield();
  }
}

void init_proc() {
	context_kload(&pcb[0], (void *)hello_fun);
	//context_uload(&pcb[1]. "/bin/init");
  switch_boot_pcb();

  Log("Initializing processes...");

  // load program
	naive_uload(NULL, "/bin/dummy");

}

_Context* schedule(_Context *prev) {
	current->cp = prev;
	//current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);
	current = &pcb[0];
	cur_c = current->cp;
	Log("cur_c = 0x%x address:0x%x", cur_c, &(current->cp->ip));
	Log("detination : 0x%x", current->cp->ip);
	return current->cp;
}
