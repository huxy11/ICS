#include "nemu.h"
#define CR0_PG 0x80000000
#define FRAME_ADDR 0xFFFFF000
#define PD_MASK 0xFFC00000
#define PD_IDX(addr) (((addr & PD_MASK) >> 22) * 4)
#define PT_MASK 0x003FF000
#define PT_IDX(addr) (((addr & PT_MASK) >> 12) * 4)
#define OF_MASK 0x00000FFF
paddr_t page_translate(vaddr_t);
paddr_t page_mechanism(vaddr_t, int);

uint32_t isa_vaddr_read(vaddr_t addr, int len) {
	addr = page_mechanism(addr, len);
	return paddr_read(addr, len);
}

void isa_vaddr_write(vaddr_t addr, uint32_t data, int len) {
	addr = page_mechanism(addr, len);
  paddr_write(addr, data, len);
}

paddr_t page_mechanism(vaddr_t addr, int len) {
	if (cpu.cr0 & CR0_PG){
		/* page mechanism */
		if ((addr / 4096) != ((addr+len-1)/4096)) {
			/* reading cross page boundary*/
			Assert(0, "cross page boundary, addr = 0x%x, addr + len = 0x%x", addr, addr+len);
		}	
		addr = page_translate(addr);
	}
	return addr;
}

paddr_t page_translate(vaddr_t addr)
{
	uint32_t pd = paddr_read(cpu.cr3 + PD_IDX(addr), 4);
	assert(pd & 1);
	uint32_t pt = paddr_read((pd & FRAME_ADDR) + PT_IDX(addr), 4); 
	Assert(pt & 1, "pt = 0x%x, pd = 0x%x, addr = 0x%x val = 0x%x", pt, pd, addr, paddr_read(0x231101, 4));
	return (pt & FRAME_ADDR) + (addr & OF_MASK);
}
