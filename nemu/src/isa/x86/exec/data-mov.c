#include "cpu/exec.h"
#define PUSHA(x)	\
	rtl_mv(&t0, concat((rtlreg_t*)&reg_, x)(R_ESP ));	\
	for (int i = R_EAX; i <= R_EDI; ++i)	\
			i != R_ESP ? rtl_push(concat((rtlreg_t*)&reg_, x)(i)) : rtl_push(&t0)
#define POPA(x)	\
	for (int i = R_EDI; i >= R_EAX; --i) \
			i != R_ESP ? rtl_pop(concat((rtlreg_t*)&reg_, x)(i)) : rtl_pop(&ir)
	

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  rtl_push(&id_dest->val);
  print_asm_template1(push);
}

make_EHelper(pop) {
	rtl_pop(&id_dest->val);
	operand_write(id_dest, &id_dest->val);
  print_asm_template1(pop);
}

make_EHelper(pusha) {
	if (decinfo.isa.is_operand_size_16) {
		PUSHA(w);
	} else {
		PUSHA(l);
		/*
		rtl_lr(&t0, R_ESP, 4);
		for(int i = R_EAX; i <= R_EDI; ++i){
			i != R_ESP ? rtl_push(&(reg_l(i))) : rtl_push(&t0);
			Log("%s : %#x", reg_name(i, 4), reg_l(i));
		}
		*/
	}
  print_asm("pusha");
}

make_EHelper(popa) {
	if (decinfo.isa.is_operand_size_16) {
		POPA(w);
	} else {
		POPA(l);
	}

  print_asm("popa");
}

make_EHelper(leave) {
	uint32_t width = decinfo.isa.is_operand_size_16 ? 2 : 4;
	rtl_lr(&s0, R_EBP, width);
	rtl_sr(R_ESP, &s0, width);
	rtl_pop(&s0);
	rtl_sr(R_EBP, &s0, width);
  print_asm("leave");
}
make_EHelper(cltd) {
	uint32_t width = decinfo.isa.is_operand_size_16 ? 2 : 4;
	rtl_lr(&s0, R_EAX, width);
	rtl_sext(&s0, &s0, width);
	if (s0 & 0x80000000)
		rtl_li(&s0, 0xFFFFFFFF);
	else
		rtl_li(&s0, 0);
	rtl_sr(R_EDX, &s0, width);
	print_asm(decinfo.isa.is_operand_size_16 ? "cwtl" : "cltd");
}

/*
make_EHelper(cltd) {
  if (decinfo.isa.is_operand_size_16) {
    TODO();
  }
  else {
    TODO();
  }

  print_asm(decinfo.isa.is_operand_size_16 ? "cwtl" : "cltd");
}
*/

make_EHelper(cwtl) {
  if (decinfo.isa.is_operand_size_16) {
    TODO();
  }
  else {
    TODO();
  }

  print_asm(decinfo.isa.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  rtl_sext(&s0, &id_src->val, id_src->width);
  operand_write(id_dest, &s0);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  operand_write(id_dest, &id_src->addr);
  print_asm_template2(lea);
}

make_EHelper(xchg) {
	rtl_lr(&t0, R_EAX, id_src->width);
	rtl_sr(R_EAX, &id_src->val, id_src->width);
	operand_write(id_src, &t0);
	print_asm_template2(xchg);
}
