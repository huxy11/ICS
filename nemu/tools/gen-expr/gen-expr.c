#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536];

static void gen_num(void)
{
	uint32_t re = rand() % 100;
	char str[32];
	switch((rand() %2)) {
		case 0: sprintf(str, "%#x", re);
						break;
		case 1: sprintf(str, "%u", re);
						break;
	}
	strcat(buf, str);
}
static void gen_op(void)
{
	switch((rand() % 7)) {
		case 0: strcat(buf, "+");
						break;
		case 1: strcat(buf, "-");
						break;
		case 2: strcat(buf, "*");
						break;
		case 3: strcat(buf, "/");
						break;
		case 4: strcat(buf, "==");
						break;
		case 5: strcat(buf, "!=");
						break;
		case 6: strcat(buf, "&&");
						break;
	}
}

static void gen_rand_expr() {
	if (strlen(buf) > 40000) {
		gen_num();
		return;
	}
	switch((rand()%3)) {
		case 0: gen_num();
						break;
		case 1: strcat(buf, "(");
						gen_rand_expr();
						strcat(buf, ")");
						break;
		default:
						gen_rand_expr();
						strcat(buf, " ");
						gen_op();
						strcat(buf, " ");
						gen_rand_expr();
						break;
	}
}

static char code_buf[65536];
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
		buf[0] = '\0';
    gen_rand_expr();
    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc -Werror /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    int tmp = fscanf(fp, "%d", &result);
		if (!tmp)
			printf("Nothing\n");
    pclose(fp);
    printf("%u %s\n", result, buf);
  }
  return 0;
}
