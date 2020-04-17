#include "nemu.h"
#include "monitor/diff-test.h"
#define CMP(a, b) if ((uint32_t)a != (uint32_t)b) { \
										Log("%s, %#x :: %#x", #a, (uint32_t)a, (uint32_t)b);\
										goto ne; \
										}
#define CMPE(x) CMP(ref_r->_ ## x, cpu._ ## x)

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
	CMP(ref_r->pc, cpu.pc)
	int32_t i;
	for(i = 0; i < 8; ++i)
		CMP(ref_r->gpr[i]._32, cpu.gpr[i]._32)
	
  return true;
ne:
	printf("error!\n");
	for (i = 0; i < 10; ++i)
		printf("%d:%#08x\n", i, ref_r->array[i]);
	return false;
}

void isa_difftest_attach(void) {
}
