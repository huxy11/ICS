#----|------------entry------------|---irq id---|-----handler-----|
.globl __am_vecsys;    __am_vecsys: pushl $0x80; jmp __am_asm_trap
.globl __am_vectrap;  __am_vectrap: pushl $0x81; jmp __am_asm_trap
.globl __am_irq0;        __am_irq0: pushl   $32; jmp __am_asm_trap
.globl __am_vecnull;  __am_vecnull: pushl   $-1; jmp __am_asm_trap
.globl current

__am_asm_trap:
  pushal 		#save regs
	
  pushl $0 	#AddressSpace as
	
#	movl current, %esi
#	movl %esp, (%esi)
	
  pushl %esp	#send in argument _Context*

  call __am_irq_handle
	
	movl %eax, %esp	#return is the new _Context* we are about to use
  
	addl $4, %esp	#AddressSpace
  popal
  addl $4, %esp #irq
  iret
