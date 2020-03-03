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
  cpu.pc = pc_sample;

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
}

void isa_reg_display(){
	int i;
	for (i = R_EAX; i < R_EDI; ++i) {
		printf("%s 0x%08x %d\n", reg_name(i, 4), reg_l(i), reg_l(i));
	}
	printf("pc:0x%08x\n", cpu.pc);

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
	/*
	for (i = 0; w; ++i) {
			printf("%s", reg_name(i, w));
		if (!strcmp(reg_name(i, w), s)) { 
			printf("Found:%s\n", reg_name(i, w));
			return 1;
		}
		if (w && i == 7) {
			w = w >> 1;
		  i = -1;
			printf("w = %u", w);
		}
	}
	*/
e:
	printf("invalid reg_name\n");
  return 0;
}
