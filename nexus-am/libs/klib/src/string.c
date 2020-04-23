#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
	size_t re = 0;
	for (; s[re] != '\0'; ++re);
  return re;
}

char *strcpy(char* dst,const char* src) {
	size_t i;
	for (i = 0; src[i]; dst[i] = src[i],++i);
  return dst;
}

char* strncpy(char* dst, const char* src, size_t n) {
	size_t i;
	for (i = 0; i < n && src[i]; dst[i] = src[i], ++i);
	for (;i < n; dst[i] = 0, ++i);
	return dst;
}

char* strcat(char* dst, const char* src) {
	size_t s = strlen(dst), o = 0;
	do dst[s+o] = src[o];
	while (src[o++]);
  return dst;
}

int strcmp(const char* s1, const char* s2) {
	return strncmp(s1, s2, (size_t)-1);
}

int strncmp(const char* s1, const char* s2, size_t n) {
	size_t i = 0;
	//compare once even if *s1 = "\0";
	do if (s1[i] != s2[i] && i < n++)
			return ((int)s1[i] - s2[i]);
	while (s1[i++]);
  return 0;
}

void* memset(void* v,int c,size_t n) {
	c = (uint8_t)c;
	for(;n > 0; --n)
		((uint8_t*)v)[n-1] = c;
  return v;
}

void* memcpy(void* out, const void* in, size_t n) {
	size_t i;
	for (i = 0; i < n ; ++i){
		((uint8_t*)out)[i] = ((uint8_t*)in)[i];
	}
  return out;
}

int memcmp(const void* s1, const void* s2, size_t n){
	size_t i;
	for (i = 0; i < n; ++i) {
		if (((uint8_t*)s1)[i] != ((uint8_t*)s2)[i])
			return (((uint8_t*)s1)[i] != ((uint8_t*)s2)[i]);
	}
  return 0;
}

#endif
