#include <am.h>
#include <amdev.h>
#include <nemu.h>
#include <klib.h>
static int h, w;

size_t __am_video_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_INFO: {
      _DEV_VIDEO_INFO_t *info = (_DEV_VIDEO_INFO_t *)buf;
			uint32_t tmp = inl(SCREEN_ADDR);
      w = info->width = tmp >> 16;
      h = info->height = tmp & 0xffff;
      return sizeof(_DEV_VIDEO_INFO_t);
    }
  }
  return 0;
}

size_t __am_video_write(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_FBCTL: {
      _DEV_VIDEO_FBCTL_t *ctl = (_DEV_VIDEO_FBCTL_t *)buf;
			int32_t *vm = (int32_t *)FB_ADDR;
			assert(ctl->y + ctl->h <= h);
			assert(ctl->x + ctl->w <= w);
			for (int j = 0; j < ctl->h; ++j)
				for (int i = 0; i < ctl->w; ++i){
					vm[w*(ctl->y + j) + (ctl->x + i)] = *ctl->pixels;
				}
      if (ctl->sync) {
        outl(SYNC_ADDR, 0);
      }
      return size;
    }
  }
  return 0;
}

void __am_vga_init() {
	int i;
	int size = screen_width() * screen_height();
	uint32_t *fb = (uint32_t*)(uintptr_t)FB_ADDR;
	for (i = 0; i < size; ++i)
		fb[i] = i;
	draw_sync();
}
