#ifndef __X86_RTL_H__
#define __X86_RTL_H__

#include "rtl/rtl.h"

/* RTL pseudo instructions */

static inline void rtl_lr(rtlreg_t* dest, int r, int width) {
  switch (width) {
    case 4: rtl_mv(dest, &reg_l(r)); return;
    case 1: rtl_host_lm(dest, &reg_b(r), 1); return;
    case 2: rtl_host_lm(dest, &reg_w(r), 2); return;
    default: assert(0);
  }
}

static inline void rtl_sr(int r, const rtlreg_t* src1, int width) {
  switch (width) {
    case 4: rtl_mv(&reg_l(r), src1); return;
    case 1: rtl_host_sm(&reg_b(r), src1, 1); return;
    case 2: rtl_host_sm(&reg_w(r), src1, 2); return;
    default: assert(0);
  }
}

static inline void rtl_push(const rtlreg_t* src1) {
  // esp <- esp - 4
  // M[esp] <- src1
  rtl_subi(&reg_l(R_ESP), &reg_l(R_ESP), 4);
  rtl_sm(&(reg_l(R_ESP)), src1, 4);
}

static inline void rtl_pop(rtlreg_t* dest) {
  // dest <- M[esp]
  // esp <- esp + 4	
	rtl_lm(dest, &reg_l(R_ESP), 4);
	rtl_addi(&reg_l(R_ESP), &reg_l(R_ESP), 4);
}



#define make_rtl_setget_eflags(f) \
  static inline void concat(rtl_set_, f) (rtlreg_t *dest) { \
    cpu._ ## f = *dest; \
  } \
	static inline void concat(rtl_set1_, f) (void) { \
		cpu._ ## f = 1; \
	}	\
	static inline void concat(rtl_reset_, f) (void) { \
		cpu._ ## f = 0; \
	} \
	static inline uint32_t concat(rtl_get1_, f) (void) { \
		return cpu._ ## f; \
	} \
  static inline void concat(rtl_get_, f) (rtlreg_t* dest) { \
    *dest = cpu._ ## f; \
  }

make_rtl_setget_eflags(CF)
make_rtl_setget_eflags(OF)
make_rtl_setget_eflags(ZF)
make_rtl_setget_eflags(SF)
make_rtl_setget_eflags(PF)
make_rtl_setget_eflags(AF)
#define s(x) (*x & s_bit) 
static inline void rtl_is_sub_overflow(rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1, const rtlreg_t* src2, int width) {
  // dest <- is_overflow(src1 - src2)
	uint32_t s_bit = (1u << (4 * width - 1));
	*dest = ((s(src1) ^ s(src2)) && (s(src1)^s(res))) ? 1 : 0;
}

static inline void rtl_is_sub_carry(rtlreg_t* dest,
    const rtlreg_t* src1, const rtlreg_t* src2) {
  // dest <- is_carry(src1 - src2)
	*dest = (src1 > src2) ? 0 : 1;
	
}

static inline void rtl_is_add_overflow(rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1, const rtlreg_t* src2, int width) {
  // dest <- is_overflow(src1 + src2)
	uint32_t s_bit = (1u << (4 * width - 1));
	*dest = ((s(src1) ^ s(src2)) || !(s(src1)^s(res))) ? 0 : 1;
}
#undef s

static inline void rtl_is_add_carry(rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1) {
	*dest = (*res < *src1) ? 1 : 0;
}

static inline void rtl_update_ZF(const rtlreg_t* result, int width) {
 	(*result) ? rtl_reset_ZF() : rtl_set1_ZF();
}

static inline void rtl_update_SF(const rtlreg_t* result, int width) {
	(*result & (1u << (width * 4 - 1))) ?  rtl_set1_SF() : rtl_reset_SF();
}

static inline void rtl_update_ZFSF(const rtlreg_t* result, int width) {
  rtl_update_ZF(result, width);
  rtl_update_SF(result, width);
}

#endif
