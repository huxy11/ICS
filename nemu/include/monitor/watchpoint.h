#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
	uint32_t val;
	char str[32];

} WP;

void wp_display(void);
WP* new_wp(const char*);
void free_wp(WP*);
void free_wp_no(int32_t);
bool wp_triggered(void);

#endif
