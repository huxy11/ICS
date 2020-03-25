static unsigned long int next = 1;
#define INT_BYTES (sizeof(int) * 4)
void itoa(int in, char* out) {
	char tmp[INT_BYTES];
	int n = 0;
	tmp[n++] = 0;
	if (in & 1<<(INT_BYTES - 1)) {
		*out++ = '-';
		in = -in;
	}
	while (in) {
		tmp[n++] = (in % 10) + '0';
		in /= 10;
	}
	n--;
	do *out++ = tmp[n];
	while(tmp[n--]);
}

int rand(void) {
  // RAND_MAX assumed to be 32767
  next = next * 1103515245 + 12345;
  return (unsigned int)(next/65536) % 32768;
}

void srand(unsigned int seed) {
  next = seed;
}
