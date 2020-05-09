#include "nemu.h"
#define CR0_PG 0x80000000
#define FRAME_ADDR 0xFFFFF000
#define PD_MASK 0xFFC00000
#define PD_IDX(addr) (((addr & PD_MASK) >> 22) * 4)
#define PT_MASK 0x003FF000
#define PT_IDX(addr) (((addr & PT_MASK) >> 12) * 4)
#define OF_MASK 0x00000FFF
#define IS_CROSS ((addr / 4096) != ((addr+len-1)/4096))
paddr_t page_translate(vaddr_t);
paddr_t page_mechanism(vaddr_t, int);

uint32_t isa_vaddr_read(vaddr_t addr, int len) {
	if (!(cpu.cr0 & CR0_PG))
		return paddr_read(addr, len);
	if (IS_CROSS) {
		int l1 = (4096 - (addr % 4096));
		int l2 = len - l1;
		uint32_t r1 = paddr_read(page_translate(addr), l1);
		uint32_t r2 = paddr_read(page_translate(addr+l1), l2);
		assert(!(r1 & 0xFF000000));
		return r1 + (r2 << (l1 * 8));
	}
	uint32_t paddr = page_translate(addr);
	return paddr_read(paddr, len);
}

void isa_vaddr_write(vaddr_t addr, uint32_t data, int len) {
	//addr = page_mechanism(addr, len);	
	if (!(cpu.cr0 & CR0_PG))
		return paddr_write(addr, data, len);
	if (IS_CROSS) {
		assert(0);
		int l1 = (4096 - (addr % 4096));
		int l2 = len - l1;
		uint32_t r = data >> (l2 * 8);
		paddr_write(page_translate(addr), r, l1);
		paddr_write(page_translate(addr+l1), data, l2);
		return;
	}
	addr = page_translate(addr);
  paddr_write(addr, data, len);
}

/*
paddr_t page_mechanism(vaddr_t addr, int len) {
	if (cpu.cr0 & CR0_PG){
		if ((addr / 4096) != ((addr+len-1)/4096)) {
			Assert(0, "cross page boundary, addr = 0x%x, addr + len = 0x%x", addr, addr+len);
		}	
		addr = page_translate(addr);
	}
	return addr;
}
*/

paddr_t page_translate(vaddr_t addr)
{
	uint32_t pd = paddr_read(cpu.cr3 + PD_IDX(addr), 4);
	Assert(pd & 1, "pd = 0x%x, addr = 0x%x, cr3 = 0x%x", pd, addr, cpu.cr3);
	uint32_t pt = paddr_read((pd & FRAME_ADDR) + PT_IDX(addr), 4); 
	Assert(pt & 1, "pt = 0x%x, pd = 0x%x, addr = 0x%x, index = 0x%x", pt, pd, addr,((pd & FRAME_ADDR) + PT_IDX(addr)) );
	return (pt & FRAME_ADDR) + (addr & OF_MASK);
}
