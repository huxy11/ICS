#include "cpu/exec.h"
//data-mov.c
make_EHelper(mov);
make_EHelper(movzx);
make_EHelper(push);
make_EHelper(pop);
make_EHelper(lea);
make_EHelper(xchg);
make_EHelper(leave);
//arith.c
make_EHelper(add);
make_EHelper(adc);
make_EHelper(sub);
make_EHelper(cmp);
make_EHelper(dec);
//control.c
make_EHelper(call);
make_EHelper(ret);
make_EHelper(jcc);
//logical.c
make_EHelper(not);
make_EHelper(and);
make_EHelper(xor);
make_EHelper(or);
make_EHelper(sar);
make_EHelper(shl);
make_EHelper(setcc);
make_EHelper(test);
//special.c
make_EHelper(nop);


make_EHelper(operand_size);

make_EHelper(inv);
make_EHelper(nemu_trap);
