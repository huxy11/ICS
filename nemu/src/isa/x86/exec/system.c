#include "cpu/exec.h"

make_EHelper(lidt) {
	rtl_lm(&s0, &id_dest->addr, 2);
	rtl_mv((rtlreg_t*)&cpu.idtr.limit, &s0);
	rtl_li(&ir, 2);
	rtl_add(&id_dest->addr, &id_dest->addr, &ir);
	rtl_lm(&s0, &id_dest->addr, id_dest->width == 2 ? 3 : 4);
	rtl_mv(&cpu.idtr.base, &s0); 
  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
  TODO();

  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  TODO();

  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

  difftest_skip_ref();
}

make_EHelper(int) {
	raise_intr(id_dest->val, decinfo.seq_pc);
  print_asm("int %s", id_dest->str);

  difftest_skip_dut(1, 2);
}

make_EHelper(iret) {
	rtl_pop(&s0);
	rtl_pop(&cpu.cs);
	rtl_pop(&cpu.eflags);

	rtl_j(s0);
	Log("s0 = %#x", s0);
  print_asm("iret");
}

uint32_t pio_read_l(ioaddr_t);
uint32_t pio_read_w(ioaddr_t);
uint32_t pio_read_b(ioaddr_t);
void pio_write_l(ioaddr_t, uint32_t);
void pio_write_w(ioaddr_t, uint32_t);
void pio_write_b(ioaddr_t, uint32_t);

make_EHelper(in) {
	switch(id_dest->width) {
		case 1: rtl_li(&t0, pio_read_b((ioaddr_t)id_src->val));break;
		case 2: rtl_li(&t0, pio_read_w((ioaddr_t)id_src->val));break;
		case 4: rtl_li(&t0, pio_read_l((ioaddr_t)id_src->val));break;
		default: assert(0);
	}
	operand_write(id_dest, &t0);
  print_asm_template2(in);
}

make_EHelper(out) {
	switch(id_src->width) {
		case 1: pio_write_b((ioaddr_t)id_dest->val, id_src->val);break;
		case 2: pio_write_w((ioaddr_t)id_dest->val, id_src->val);break;
		case 4: pio_write_l((ioaddr_t)id_dest->val, id_src->val);break;
	}
  print_asm_template2(out);
}
