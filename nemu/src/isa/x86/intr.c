#include "rtl/rtl.h"

#define IDT_READ(NO, idte_lo, idte_hi, offset) \
	assert((NO * 2 + 2) * 4 <= cpu.idtr.limit);	\
	idte_lo = vaddr_read(cpu.idtr.base + (NO * 2) * 4, 4);	\
	idte_hi = vaddr_read(cpu.idtr.base + (NO * 2 + 1) * 4, 4);	\
	assert(idte_hi & 0x8000);	\
	offset = (idte_lo & 0xffff) + (idte_hi & 0xffff0000)

void raise_intr(uint32_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
	uint32_t idte_lo, idte_hi, offset;
	IDT_READ(NO, idte_lo, idte_hi, offset);
	rtl_push(&cpu.eflags);
	rtl_push(&cpu.cs);
	rtl_push(&ret_addr);
	rtl_mv(&s0, &offset);
	rtl_jr(&s0);
}

bool isa_query_intr(void) {
  return false;
}

#undef IDT_READ 
