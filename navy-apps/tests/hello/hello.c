#include <unistd.h>
#include <stdio.h>

int main() {
  int i = 2;
  int j = 0;
  while (i) {
    j ++;
    if (j == 10000) {
      printf("Hello World from Navy-apps for the %dth time!\n", i ++);
      j = 0;
    }
  }
  return 0;
}
