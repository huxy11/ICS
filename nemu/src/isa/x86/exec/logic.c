#include "cpu/exec.h"
#include "cc.h"
static void _eflags(const rtlreg_t* dest, uint32_t width) {
	rtl_update_ZFSF(dest, width);
	rtl_reset_CF();
	rtl_reset_OF();
}

#define procedure(op)  \
	rtl_sext(&id_src->val, &id_src->val, id_src->width); \
	rtl_sext(&id_dest->val, &id_dest->val, id_dest->width); \
	id_src->width = id_dest->width; \
	rtl_ ## op(&id_dest->val, &id_dest->val, &id_src->val); \
	operand_write(id_dest, &id_dest->val); \
	_eflags(&id_dest->val, id_dest->width); \
  print_asm_template2(op); \


make_EHelper(test) {
	rtl_and(&id_dest->val, &id_dest->val, &id_src->val);
	_eflags(&id_dest->val, id_dest->width);
  print_asm_template2(test);
}

make_EHelper(and) {
	/*
	rtl_sext(&id_src->val, &id_src->val, id_src->width);
	rtl_sext(&id_dest->val, &id_dest->val, id_dest->width);
	id_src->width = id_dest->width;
	rtl_and(&id_dest->val, &id_dest->val, &id_src->val);
	operand_write(id_dest, &id_dest->val);
	_eflags(&id_dest->val, id_dest->width);
  print_asm_template2(and);
	*/
	procedure(and)

}

make_EHelper(xor) {
	/*
	rtl_sext(&id_src->val, &id_src->val, id_src->width);
	id_src->width = id_dest->width;
	rtl_xor(&id_dest->val, &id_dest->val, &id_src->val);
	operand_write(id_dest, &id_dest->val);
	_eflags(&id_dest->val, id_dest->width);
  print_asm_template2(xor);
	*/
	procedure(xor)
}

make_EHelper(or) {
	/*
	rtl_sext(&id_src->val, &id_src->val, id_src->width);
	id_src->width = id_dest->width;
	rtl_or(&id_dest->val, &id_dest->val, &id_src->val);
	operand_write(id_dest, &id_dest->val);
	_eflags(&id_dest->val, id_dest->width);
  print_asm_template2(or);
	*/
	procedure(or)
}

make_EHelper(sar) {
	rtl_sar(&id_dest->val, &id_dest->val, &id_src->val); 
	operand_write(id_dest, &id_dest->val);
	rtl_update_ZFSF(&id_dest->val, id_dest->width);
  // unnecessary to update CF and OF in NEMU
  print_asm_template2(sar);
}

make_EHelper(shl) {
	rtl_shl(&id_dest->val, &id_dest->val, &id_src->val);
	operand_write(id_dest, &id_dest->val);
	rtl_update_ZFSF(&id_dest->val, id_dest->width);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shl);
}

make_EHelper(shr) {
	rtl_shr(&id_dest->val,  &id_dest->val, &id_src->val);
	operand_write(id_dest, &id_dest->val);
	rtl_update_ZFSF(&id_dest->val, id_dest->width);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint32_t cc = decinfo.opcode & 0xf;
  rtl_setcc(&s0, cc);
  operand_write(id_dest, &s0);

  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}

make_EHelper(not) {
	rtl_not(&id_dest->val, &id_dest->val);
	operand_write(id_dest, &id_dest->val);

  print_asm_template1(not);
}
#undef procedure
