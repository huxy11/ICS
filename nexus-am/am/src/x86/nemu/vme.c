#include <am.h>
#include <x86.h>
#include <nemu.h>
#include <klib.h>

#define PG_ALIGN __attribute((aligned(PGSIZE)))
#define EFLAGS_IF (1 << 9)
#define EFLAGS_DEFAULT (1 << 1)

static PDE kpdirs[NR_PDE] PG_ALIGN = {};
static PTE kptabs[(PMEM_SIZE + MMIO_SIZE) / PGSIZE] PG_ALIGN = {};
static void* (*pgalloc_usr)(size_t) = NULL;
static void (*pgfree_usr)(void*) = NULL;
static int vme_enable = 0;
static _AddressSpace *cur_as = NULL;

static _Area segments[] = {      // Kernel memory mappings
  {.start = (void*)0,          .end = (void*)PMEM_SIZE},
  {.start = (void*)MMIO_BASE,  .end = (void*)(MMIO_BASE + MMIO_SIZE)}
};

#define NR_KSEG_MAP (sizeof(segments) / sizeof(segments[0]))

int _vme_init(void* (*pgalloc_f)(size_t), void (*pgfree_f)(void*)) {
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  int i;

  // make all PDEs invalid
  for (i = 0; i < NR_PDE; i ++) {
    kpdirs[i] = 0;
  }

  PTE *ptab = kptabs;
  for (i = 0; i < NR_KSEG_MAP; i ++) {
    uint32_t pdir_idx = (uintptr_t)segments[i].start / (PGSIZE * NR_PTE);
    uint32_t pdir_idx_end = (uintptr_t)segments[i].end / (PGSIZE * NR_PTE);
    for (; pdir_idx < pdir_idx_end; pdir_idx ++) {
      // fill PDE
      kpdirs[pdir_idx] = (uintptr_t)ptab | PTE_P;

      // fill PTE
      PTE pte = PGADDR(pdir_idx, 0, 0) | PTE_P;
      PTE pte_end = PGADDR(pdir_idx + 1, 0, 0) | PTE_P;
      for (; pte < pte_end; pte += PGSIZE) {
        *ptab = pte;
        ptab ++;
      }
    }
  }

  set_cr3(kpdirs);
  set_cr0(get_cr0() | CR0_PG);
  vme_enable = 1;
  return 0;
}

int _protect(_AddressSpace *as) {
	assert(!(as->ptr));
  PDE *updir = (PDE*)(pgalloc_usr(1));
  as->ptr = updir;
  // map kernel space
  for (int i = 0; i < NR_PDE; i ++) {
    updir[i] = kpdirs[i];
  }

  return 0;
}

void _unprotect(_AddressSpace *as) {
}

void __am_get_cur_as(_Context *c) {
	if (!cur_as) {
		cur_as = (void*)0x209004;
		cur_as->ptr = (void*)get_cr3();
	}
  c->as = cur_as;
}

void __am_switch(_Context *c) {
  if (vme_enable) {
    set_cr3(c->as->ptr);
    cur_as = c->as;
  }
}

int _map(_AddressSpace *as, void *va, void *pa, int prot) {
	//assert(OFF(va) == OFF(pa)); 
	assert(as && as->ptr);
	PDE *updir = as->ptr;
	PTE *uptab;
	/* Set PDE */
	if (!(updir[PDX(va)] & PTE_P)) {
		/* new page table */
		uptab = (PTE*)(pgalloc_usr(1));
		updir[PDX(va)] = (((uint32_t)uptab & 0xFFFFF000) | PTE_P);
	}
	/* Set PTE */
	uptab = (PTE*)(((uint32_t)updir[PDX(va)]) & 0xFFFFF000);

	assert((!(uptab[PTX(va)] & PTE_P) || \
				((uptab[PTX(va)] & 0xFFFFF000) == ((uint32_t)pa & 0xFFFFF000)) ));

	uptab[PTX(va)] = ((uint32_t)pa & 0xFFFFF000) | PTE_P;	
  return 0;
}

_Context *_ucontext(_AddressSpace *as, _Area ustack, _Area kstack, void *entry, void *args) {
	_Context *c = (ustack.end - 1) - sizeof(_Context) - 4*sizeof(int);
	//_protect(as);
	memset(c, 0 ,sizeof(_Context));
	c->ip = (uint32_t)entry;
	c->cs = 8;
	/* set IF*/
	c->eflags = EFLAGS_DEFAULT | EFLAGS_IF;
	c->esp = (uint32_t)ustack.end - 1 - 4*sizeof(int);
	c->as = as;
  return c;
}
