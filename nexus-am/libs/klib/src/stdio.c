#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
static char _re[sizeof(int) * 8] = {0, };
static char* _itoa(int in) {
	assert(in >= 0);
	int i = sizeof(_re) - 1;
	do {
		_re[--i] = in % 10 + '0';
		in /= 10;
	} while(in);
	return &_re[i];	
}
static char* _itox(unsigned in) {
	char _hex[] = {'0', '1', '2', '3', '4', '5', '6', '7', \
		'8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
	int i = sizeof(_re) - 1;
	do {
		_re[--i] = _hex[in % 16];
		in /= 16;
	} while (in);
	return &_re[i];
}

int printf(const char *fmt, ...) {
	va_list ap;
	char c;
	int d;
	unsigned u;
	const char* base = fmt;
	char* s;
	va_start(ap, fmt);
	while (*fmt) {
		assert(fmt - base <0xffff);
		if (*fmt == '%') {
			switch(*++fmt) {
				case 'c': c = va_arg(ap, int);
									_putc((char)c);
									break;
				case 's': s = va_arg(ap, char *);
									while(*s)
										_putc(*s++);
									break;
				case 'd':	d = va_arg(ap, int);
									if (d < 0) {
										_putc('-');
										d = -d;
									}
									s = _itoa(d);
									while(*s)
										_putc(*s++);
									break;
				case '#': assert(*++fmt == 'x');
									_putc('0');	
									_putc('x');
				case 'p':
				case 'x':	u = va_arg(ap, unsigned);
									s = _itox(u);
									while (*s)
										_putc(*s++);
									break;
				default:
									printf("\n%c to be implemented!\n", *fmt);
									assert(0);
			}
			fmt++;
		} else {
			_putc(*fmt++);
		}
	}
  return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  return 0;
}

int sprintf(char *out, const char *fmt, ...) {
	va_list ap;
	int d;
	const char* base = fmt;
	//char ds[sizeof(int) * 8];
	char* s;
	va_start(ap, fmt);
	while (*fmt) {
		assert(fmt - base <0xffff);
		if (*fmt == '%') {
			switch(*++fmt) {
				case 's': s = va_arg(ap, char *);
									while(*s)
										*out++ = *s++;
									break;
				case 'd':	d = va_arg(ap, int);
									if (d < 0) {
										_putc('-');
										d = -d;
									}
									s = _itoa(d);
									while(*s)
										_putc(*s++);
									break;
									/*
				case 'd':	d = va_arg(ap, int);
									itoa(d, ds);
									d = 0;
									while(ds[d])
										*out++ = ds[d++];
									break;
									*/
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
