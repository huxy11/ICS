#ifndef __X86_REG_H__
#define __X86_REG_H__

#include "common.h"

#define PC_START IMAGE_START

enum { R_EAX, R_ECX, R_EDX, R_EBX, R_ESP, R_EBP, R_ESI, R_EDI };
enum { R_AX, R_CX, R_DX, R_BX, R_SP, R_BP, R_SI, R_DI };
enum { R_AL, R_CL, R_DL, R_BL, R_AH, R_CH, R_DH, R_BH };

typedef union  {
	struct {
		union {
			struct {
				union {
					uint32_t _32;
					uint16_t _16;
					uint8_t _8[2];
				};
			} gpr[8];
			struct {
			rtlreg_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
			};
		};
		vaddr_t pc;
		union{
			uint32_t eflags;
			struct {
				uint32_t _CF:1;
				uint32_t :1;
				uint32_t _PF:1;
				uint32_t :1;
				uint32_t _AF:1;
				uint32_t :1;
				uint32_t _ZF:1;
				uint32_t _SF:1;
				uint32_t _TF:1;
				uint32_t _IF:1;
				uint32_t _DF:1;
				uint32_t _OF:1;
				uint32_t _IOPL:2;
				uint32_t _NT:1;
				uint32_t :1;
				uint32_t _RF:1;
				uint32_t _VM:1;
			};
		};
		uint32_t cs;
	};
	uint32_t array[77];
} CPU_state;

static inline int check_reg_index(int index) {
  assert(index >= 0 && index < 8);
  return index;
}

#define reg_l(index) (cpu.gpr[check_reg_index(index)]._32)
#define reg_w(index) (cpu.gpr[check_reg_index(index)]._16)
#define reg_b(index) (cpu.gpr[check_reg_index(index) & 0x3]._8[index >> 2])

static inline const char* reg_name(int index, int width) {
  extern const char* regsl[];
  extern const char* regsw[];
  extern const char* regsb[];
  assert(index >= 0 && index < 8);

  switch (width) {
    case 4: return regsl[index];
    case 1: return regsb[index];
    case 2: return regsw[index];
    default: assert(0);
  }
}

void isa_reg_display(void);
uint32_t isa_reg_str2val(const char*);

#endif
