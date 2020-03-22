#include "nemu.h"

const uint8_t isa_default_img []  = {
  0xb8, 0x00, 0x00, 0x00, 0x00,        // 100000:  movl  $0x0,%eax
  0xe8, 0x01, 0x00, 0x00, 0x00,		   // 100005:  call 0x10000b
  0xd6,                                // 10000a:  nemu_trap
  0xb8, 0x34, 0x12, 0x00, 0x00,        // 10000b:  movl  $0x1234,%eax
  0x55,								  							 // 100010:  push
	0x50,
	0x51,
  0xd6,                                // 100013:  nemu_trap
};
const long isa_default_img_size = sizeof(isa_default_img);

static void restart() {
  /* Set the initial program counter. */
  cpu.pc = PC_START;
}

void init_isa(void) {
  /* Test the implementation of the `CPU_state' structure. */
  void reg_test(void);
  reg_test();

  /* Setup physical memory address space. */
  register_pmem(0);

  /* Initialize this virtual computer system. */
  restart();
}
