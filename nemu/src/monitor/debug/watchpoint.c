#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head_ = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head_ = NULL;
  free_ = wp_pool;
}
WP *new_wp(const char* str)
{
	if (!free_) {
		printf("No available watchpoint\n");
		goto e;
	}
	bool success;
	uint32_t re = expr(str, &success);
	if (!success)
		goto e;
	//New watchpoint
	WP *cur = free_;
	free_ = free_->next;
	strcpy(cur->str, str);
	cur->val = re;
	cur->next = NULL;
	//Insert into list head_	
	WP **ptr = &head_;
	while (*ptr)
		ptr = &((*ptr)->next);
	*ptr = cur;
	return cur;
e:
	printf("Watch point building failed\n");
	return NULL;
}
void free_wp_no(int32_t i)
{
	if ( i >= NR_WP)
		return;
	free_wp(&wp_pool[i]);
}
void free_wp(WP *wp)
{
	if (!wp) {
		Log("NULL pointer");
		return;
	}
	WP **ptr = &head_;
	//remove from _head_
	while (*ptr && *ptr != wp) {
		ptr = &((*ptr)->next);
	}	
		//wp is not in list head_
	if (!(*ptr))
		return;

	*ptr = wp->next;
	wp->str[0] = '\0';
	wp->next = NULL;
	ptr = &free_;
	while (*ptr)
		ptr = &((*ptr)->next);
	*ptr = wp;
	return;
}
static void free_display(void)__attribute__((used));
void wp_display(void)
{
	WP *cur = head_;
	int32_t i = 0;
	while (cur) {
		printf("%d:%s%c", cur->NO, cur->str, i % 4 == 3 ? '\n' : ' ');
		cur = cur->next;
		++i;
	}
	if (i % 4 != 0)
		printf("\n");
	return;
}
static void free_display(void)
{
	WP *cur = free_;
	int32_t i = 0;
	printf("-----free-------\n");
	while (cur) {
		printf("%d:%s%c", cur->NO, cur->str, i % 4 == 3 ? '\n' : ' ');
		cur = cur->next;
		++i;
	}
	if (i % 4 != 0)
		printf("\n");
	printf("----------------\n");
	return;
}

static bool _triggered(WP *wp)
{
	bool res;
	uint32_t re = expr(wp->str, &res);
	res = (re != wp->val);
	if (res) {
		printf("WP%d:%s triggered. %u, %#x --> %u, %#x\n", \
				wp->NO, wp->str, wp->val, wp->val, re, re);
		wp->val = re;
	}
	return res;	
}
bool wp_triggered(void)
{
	WP *cur = head_;
	bool re = false; 
	while (cur){ 
		if (_triggered(cur))
			re = true;	
		cur = cur->next;
	}
	return re;
}
