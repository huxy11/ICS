#include <stdint.h>

#define SYS_yield 1
extern int _syscall_(int, uintptr_t, uintptr_t, uintptr_t);

int main() {
	int j = 0;
	while(++j < 50) {
		//write(1, "Hello world!", 13);
		printf("Dummy : %d\n", j);
		_syscall_(SYS_yield, 0, 0, 0);
	}
	return 0;	
}
