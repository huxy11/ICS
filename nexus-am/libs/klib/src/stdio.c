#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  return 0;
}

int sprintf(char *out, const char *fmt, ...) {
	va_list ap;
	int d;
	const char* base = fmt;
	char ds[sizeof(int) * 8];
	char* s;
	va_start(ap, fmt);
	while (*fmt) {
		if (*fmt == '%') {
			switch(*++fmt) {
				case 's': s = va_arg(ap, char *);
									while(*s)
										*out++ = *s++;
									break;
				case 'd':	d = va_arg(ap, int);
									itoa(d, ds);
									d = 0;
									while(ds[d])
										*out++ = ds[d++];
									break;
			}
			fmt++;
		} else {
			*out++ = *fmt++;
		}
	}
	*out = 0;
	return fmt - base + 1;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

#endif
