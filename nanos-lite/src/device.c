#include "common.h"
#include <amdev.h>
#include <klib.h>
#include "fs.h"

size_t serial_write(const void *buf, size_t offset, size_t len) {
	_yield();
	int i;
	for (i = 0;i < len; ++i){
		char c = *((char*)buf + i);
		_putc(c);
	}
  return i;
}

#define NAME(key) \
  [_KEY_##key] = #key,


static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t offset, size_t len) {
	_yield();
	int ret;
	ret = read_key();
	if ((ret&0x7fff) != _KEY_NONE){
		sprintf(buf,"k%c %s\n", ret&0x8000 ? 'd' : 'u', keyname[ret&0x7fff]);
		return strlen(buf);
	}
	ret = uptime();
	sprintf(buf,"t %d\n", ret);
  return strlen(buf);
}

static char dispinfo[128] = {};

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
	size_t l = strlen(strncpy(buf, dispinfo+offset, len));
	assert(l < len);
	return l;
}
static int screen_w, screen_h;
size_t fb_write(const void *buf, size_t offset, size_t len) {
	_yield();
	intptr_t p = (intptr_t)buf;
	uint32_t *pixels = (uint32_t *)p;
	int x = offset / 4 % screen_w;
	int y = offset / 4 / screen_w;
	draw_rect(pixels, x, y, len/sizeof(int), 1);	
  return len;
}

size_t fbsync_write(const void *buf, size_t offset, size_t len) {
	draw_sync();
  return 0;
}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();
	sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d\0", screen_w = screen_width(), screen_h = screen_height());
	Log("dispinfo = %s dispinfo'address = %#x", dispinfo, &dispinfo);
  // described in the Navy-apps convention
}
size_t screen_size() {
	return screen_w * screen_h;
}
