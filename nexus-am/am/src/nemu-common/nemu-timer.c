#include <am.h>
#include <amdev.h>
#include <nemu.h>
#include <klib.h>
_DEV_TIMER_UPTIME_t uptime_base;

size_t __am_timer_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_TIMER_UPTIME: {
      _DEV_TIMER_UPTIME_t *uptime = (_DEV_TIMER_UPTIME_t *)buf;
      uptime->hi = 0;
			uint32_t tmp = inl(RTC_ADDR);
			assert(tmp >= uptime_base.lo);
      uptime->lo = tmp - uptime_base.lo;
      return sizeof(_DEV_TIMER_UPTIME_t);
    }
    case _DEVREG_TIMER_DATE: {
      _DEV_TIMER_DATE_t *rtc = (_DEV_TIMER_DATE_t *)buf;
      rtc->second = 0;
      rtc->minute = 0;
      rtc->hour   = 0;
      rtc->day    = 0;
      rtc->month  = 0;
      rtc->year   = 2000;
      return sizeof(_DEV_TIMER_DATE_t);
    }
  }
  return 0;
}

void __am_timer_init() {
	uptime_base.hi = 0;
	uptime_base.lo = inl(RTC_ADDR);
}


