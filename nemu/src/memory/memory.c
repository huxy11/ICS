#include "nemu.h"
#include "device/map.h"

uint8_t pmem[PMEM_SIZE] PG_ALIGN = {};

static IOMap pmem_map = {
  .name = "pmem",
  .space = pmem,
  .callback = NULL
};

void register_pmem(paddr_t base) {
  pmem_map.low = base;
  pmem_map.high = base + PMEM_SIZE - 1;

  Log("Add '%s' at [0x%08x, 0x%08x]", pmem_map.name, pmem_map.low, pmem_map.high);
}

IOMap* fetch_mmio_map(paddr_t addr);

/* Memory accessing interfaces */

uint32_t paddr_read(paddr_t addr, int len) {
  if (map_inside(&pmem_map, addr)) {
    uint32_t offset = addr - pmem_map.low;
    return *(uint32_t *)(pmem + offset) & (~0u >> ((4 - len) << 3));
  }
  else {
    return map_read(addr, len, fetch_mmio_map(addr));
  }
}

void paddr_write(paddr_t addr, uint32_t data, int len) {
  if (map_inside(&pmem_map, addr)) {
    uint32_t offset = addr - pmem_map.low;
    memcpy(pmem + offset, &data, len);
  }
  else {
    return map_write(addr, data, len, fetch_mmio_map(addr));
  }
}

uint64_t atox(const char *s){
	int len = strlen(s);
	int64_t i;
	uint64_t re = 0;
	for (i = 0;  i < len; ++i) {
		if (s[i] <= '9' && s[i] >= '0'){
			re *= 16;
			re += (s[i] & 0xF);
		} else if ((s[i] <='z' && s[i] >='a') || (s[i] <= 'Z' && s[i] >='A')){
			re *= 16;
			re += ((s[i] & 0xF) + 9);
		} else{
			printf("%c is invalid, returning zero.\n", s[i]); 
			return 0;
		}
	}	
	return re;

}
