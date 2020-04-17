#include "trap.h"
int foo3(int a, int b) {
	if (b >= a)
		return b - a;
	return 0;
}
int foo2(int a, int b) {
	if (a > b)
		return a - b;
	return 0;
}
int foo(int a, int b){
	if (a >b)
		return foo2(a, b);
	else 
		return foo3(a, b);
}
int main() {
	int i = 0, j = 2;
	nemu_assert((i + 1)== 1);
	i = 3;
	i = foo(i, j);
	return 0;
}
