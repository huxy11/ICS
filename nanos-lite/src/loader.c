#include "proc.h"
#include "fs.h"
#include "memory.h"
#include <elf.h>
#include <klib.h>

#ifdef __ISA_AM_NATIVE__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

static uintptr_t loader(_AddressSpace *as, const char *filename) {
	assert(as);
	printf("as->ptr = 0x%x\n", (uint32_t)as->ptr);
	Elf_Ehdr ehdr;
	Elf_Phdr phdr[64];
	int fd = fs_open(filename, 0, 0);
	assert(fd);
	/* alloc physical page frame */
	/* set open_offset at the beginning */
	fs_lseek(fd, 0, 0);
	fs_read(fd, &ehdr, sizeof(Elf_Ehdr));
	assert(ehdr.e_ident[EI_MAG0] == ELFMAG0);
	assert(ehdr.e_ident[EI_MAG3] == ELFMAG3);
	assert(ehdr.e_phoff);
	assert(ehdr.e_phentsize == sizeof(Elf_Phdr));
	/* loading phdr */
	fs_lseek(fd, ehdr.e_phoff, 0);
	fs_read(fd, &phdr, ehdr.e_phnum * ehdr.e_phentsize);
	/* loading necessary entries */
	for (int i = 0; i < ehdr.e_phnum; ++i) {
		if (phdr[i].p_type == PT_LOAD) {
			size_t off = phdr[i].p_offset;
			size_t fsz = phdr[i].p_filesz;
			size_t msz = phdr[i].p_memsz;
			void *p = (void*)(phdr[i].p_paddr);
			/* number of needed pages*/
			int pcnt = phdr[i].p_memsz/PGSIZE + (phdr[i].p_memsz%PGSIZE ? 1 : 0);
			void *physic_pg = new_page(pcnt);	
			for (int i = 0; i < pcnt; ++i)
				_map(as, p + i * PGSIZE, physic_pg + i * PGSIZE, 0);
			memset(physic_pg + ((uint32_t)p & 0xFFF), 0, msz + 4);
			fs_lseek(fd, off, 0);
			fs_read(fd, physic_pg + ((uint32_t)p & 0xFFF), fsz);
		}
	}
	return ehdr.e_entry;
}

void naive_uload(_AddressSpace *as, const char *filename) {
	assert(as);
  uintptr_t entry = loader(as, filename);
  Log("Jump to entry = %x", entry);
  ((void(*)())entry) ();
}

static inline uint32_t get_cr3() {
	volatile uint32_t val;
	asm volatile ("movl %%cr3, %0" : "=r"(val));
	return val;
}
void context_kload(PCB *pcb, void *entry) {
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);
  pcb->cp = _kcontext(stack, entry, NULL);
}
void context_uload(PCB *pcb, const char *filename) {
	if(!(pcb->as.ptr))
		_protect(&(pcb->as));
  uintptr_t entry = loader(&pcb->as, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
