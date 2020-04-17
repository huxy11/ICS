#include "nemu.h"
#include <stdlib.h>
#include <time.h>

const char *regsl[] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};
const char *regsw[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
const char *regsb[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};
void reg_test() {
  srand(time(0));
  uint32_t sample[8];
  uint32_t pc_sample = rand();
	uint32_t eflags_sample = rand();
  cpu.pc = pc_sample;
	cpu.eflags = eflags_sample;

  int i;
  for (i = R_EAX; i <= R_EDI; i ++) {
    sample[i] = rand();
    reg_l(i) = sample[i];
    assert(reg_w(i) == (sample[i] & 0xffff));
  }

  assert(reg_b(R_AL) == (sample[R_EAX] & 0xff));
  assert(reg_b(R_AH) == ((sample[R_EAX] >> 8) & 0xff));
  assert(reg_b(R_BL) == (sample[R_EBX] & 0xff));
  assert(reg_b(R_BH) == ((sample[R_EBX] >> 8) & 0xff));
  assert(reg_b(R_CL) == (sample[R_ECX] & 0xff));
  assert(reg_b(R_CH) == ((sample[R_ECX] >> 8) & 0xff));
  assert(reg_b(R_DL) == (sample[R_EDX] & 0xff));
  assert(reg_b(R_DH) == ((sample[R_EDX] >> 8) & 0xff));

  assert(sample[R_EAX] == cpu.eax);
  assert(sample[R_ECX] == cpu.ecx);
  assert(sample[R_EDX] == cpu.edx);
  assert(sample[R_EBX] == cpu.ebx);
  assert(sample[R_ESP] == cpu.esp);
  assert(sample[R_EBP] == cpu.ebp);
  assert(sample[R_ESI] == cpu.esi);
  assert(sample[R_EDI] == cpu.edi);

  assert(pc_sample == cpu.pc);

	assert(cpu.eflags == eflags_sample);
	assert(cpu._CF == (eflags_sample & 1));
	assert(cpu._PF == (eflags_sample & 1 << 2) >> 2); 
	assert(cpu._AF == (eflags_sample & 1 << 4) >> 4); 
	assert(cpu._ZF == (eflags_sample & 1 << 6) >> 6); 
	assert(cpu._SF == (eflags_sample & 1 << 7) >> 7); 
	assert(cpu._TF == (eflags_sample & 1 << 8) >> 8); 
	assert(cpu._IF == (eflags_sample & 1 << 9) >> 9); 
	assert(cpu._DF == (eflags_sample & 1 << 10) >> 10); 
	assert(cpu._OF == (eflags_sample & 1 << 11) >> 11); 
	assert(cpu._IOPL == (eflags_sample & 3 << 12) >> 12); 
	assert(cpu._NT == (eflags_sample & 1 << 14) >> 14); 
	assert(cpu._RF == (eflags_sample & 1 << 16) >> 16); 
	assert(cpu._VM == (eflags_sample & 1 << 17) >> 17); 

	cpu.eflags = 0x7fff;
  for (i = R_EAX; i <= R_EDI; i ++) 
		cpu.gpr[i]._32 = 0;

	
	Log("Reg test completed.");
}

void isa_reg_display(){
	int i;
	for (i = R_EAX; i <= R_EDI; ++i) {
		printf("%s 0x%08x %d\n", reg_name(i, 4), reg_l(i), reg_l(i));
	}
	printf("pc:0x%08x\n", cpu.pc);
	printf("SF: %u\n", cpu._SF);
	printf("ZF: %u\n", cpu._ZF);
	printf("OF: %u\n", cpu._OF);
	printf("CF: %u\n", cpu._CF);
}

uint32_t isa_reg_str2val(const char *s) {
	if (!s)
		goto e;
	if (strcmp(s, "pc") == 0)
		return cpu.pc;
	uint32_t i, w = 4;
	for (;w;) {
		for (i = 0; i < 7; ++i) 
			if (!strcmp(reg_name(i, w), s)) {
				printf("Found:%s\n", reg_name(i, w));	
				switch(w) {
					case 4: return reg_l(i);
					case 2: return reg_w(i);
					case 1: return reg_b(i);
					default: assert(0);
				}
			}
		w = w >> 1;
	}
e:
	printf("invalid reg_name\n");
  return 0;
}
