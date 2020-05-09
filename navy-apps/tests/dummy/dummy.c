#include <stdint.h>

#define SYS_yield 1
extern int _syscall_(int, uintptr_t, uintptr_t, uintptr_t);

int main() {
	int i = 1,j = 0;
	while(++j) {
		if (j == 1000) {
			//write(1, "Hello world!", 13);
			printf("Dummy : %d\n", i++);
			//_syscall_(SYS_yield, 0, 0, 0);
			j = 0;
		}
	}
	return 0;	
}
