#include <unistd.h>
#include <stdio.h>

int main() {
	int re = 0;
  re = write(1, "Hello World!\n", 13);
	write(1, "HELLO WORLD!\n", 13);
	printf("re = %d\n", re);
  int i = 2;
  volatile int j = 0;
  while (i <= 100) {
    j ++;
    if (j == 10000) {
      printf("Hello World from Navy-apps for the %dth time!\n", i ++);
      j = 0;
    }
  }
  return 0;
}
