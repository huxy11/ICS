#include "proc.h"
#include <elf.h>
//#include <stdlib.h>
#include <klib.h>

#ifdef __ISA_AM_NATIVE__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

static uintptr_t loader(PCB *pcb, const char *filename) {
	Elf_Ehdr ehdr;
	Elf_Phdr phdr[64];
	size_t fstart = 0;
	ramdisk_read(&ehdr, fstart, sizeof(Elf_Ehdr));
	assert(ehdr.e_ident[EI_MAG0] == ELFMAG0);
	assert(ehdr.e_ident[EI_MAG3] == ELFMAG3);
	assert(ehdr.e_phoff);
	assert(ehdr.e_phentsize == sizeof(Elf_Phdr));
//	phdr = malloc(ehdr.e_phnum * ehdr.e_phentsize);
	ramdisk_read(&phdr, ehdr.e_phoff, ehdr.e_phnum * ehdr.e_phentsize);
	for (int i = 0; i < ehdr.e_phnum; ++i){
		if (phdr[i].p_type == PT_LOAD) {
			void *p = (void*)(phdr[i].p_paddr);
			size_t off = phdr[i].p_offset;
			size_t fsz = phdr[i].p_filesz;
			size_t msz = phdr[i].p_memsz;
			printf("p = %p, off = %#x, fsz = %#x, msz = %#x\n", p, off, fsz, msz);
			memset(p, 0, msz);
			ramdisk_read(p, fstart + off, fsz);
			printf("loaded!\n");
		}
	}
  return ehdr.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %x", entry);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void *entry) {
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
