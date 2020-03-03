#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}
static int cmd_si(char *args) {
	char* arg = strtok(NULL, " ");
	int n = 1;
	if (arg && atoi(arg))
		n = atoi(arg);
	else 
		Log("Invalid arg for cmd_si execute once.");
	cpu_exec(n);
	return 0;
}
static int cmd_i(char* args) {
	char *arg = strtok(NULL, " ");
	if (arg) {
		if ( strcmp(arg, "r") == 0) {
			isa_reg_display();
		} else if (strcmp(arg, "wp") == 0) {
			printf("Watch points:\n");
			wp_display();
		}	
	}
	return 0;
}
static int cmd_x(char *args) {
	char *arg = strtok(NULL, " ");
	if (arg && atoi(arg)) {
		int n = atoi(arg);
		arg = strtok(NULL, " ");
		if (arg && strncmp(arg, "0x", 2) == 0) {
			arg += 2;
			if (arg && atoi(arg)) {
				uint64_t addr = atox(arg);
				int i = 0;
				for (i = 0; i < n; ++i) {
					printf("0x%lX:%02x%c", (addr+i),vaddr_read(addr+i, 1), \
							((i+1) % 8 == 0 || i == n -1) ? '\n' : ' ');	
				}
				return 0;
			}
		}
	}
	printf("Invalid args for command:x");
	return 0;
}
static int cmd_reg(char *args) {
	char *arg = strtok(NULL, " ");
	if (arg) {
		uint32_t re = isa_reg_str2val(arg);
		printf("%#x  %u\n", re, re);
	}
	return 0;
}
static int cmd_expr(char *args) {
	bool success;
	if (args) {
		printf("result = %u, %#x\n", expr(args, &success), expr(args, &success));
	}	
	return 0;
}
static void expr_autotest() {
	FILE* f = fopen("/tmp/.input", "r");
	Assert(f, "fopen failed!");
	uint32_t re;
	bool success;
	char e[65535];
	int r = 1, cnt = 0, mch = 0;
	while( r !=  EOF && r != 0) {
		++cnt;
		Log("%d:", cnt);
		r = fscanf(f, "%u %[^\n]", &re, e);
		uint32_t cmp = expr(e, &success);
		if (!(success && (cmp == re)))
			Log("%s\n Not matched, expect:%u, we got:%u", e, re, cmp);
		else {
			Log("check!");
			++mch;
		}
	}
	if (r == 0)
		printf("r = 0\n");
	if (r == EOF)
		printf("EOF\n");
	printf("Test completed, %d/%d matched.\n", mch, cnt);
	fclose(f);
}
static int cmd_wp(char* args) {
	char *arg = strtok(NULL, " ");
	if (arg) {
		new_wp(arg);
	}
	return 0;
}
static int cmd_dwp(char* args) {
	char *arg = strtok(NULL, " ");
	if (arg) {
		int32_t i = atoi(arg);
		free_wp_no(i);	
	}
	return 0;
}
static int cmd_test(char *args) {
	char *arg = strtok(NULL, " ");
	if (arg) {
		if (strcmp(arg, "reg_init") == 0) {
			cpu.eax = 0;
			cpu.edx = 0xF; 
			printf("Set EAX to 0, EDX to 0xF\n");
		} else if (strcmp(arg, "mem_init") == 0) {
			vaddr_write(0x80000, 0xAAAAFFFF, 4);
			printf("vaddr_write 0xAAAAFFFF to 0x80000\n");
		} else if (strcmp(arg, "expr") == 0) {
			expr_autotest();
		} else if (strcmp(arg, "wp") == 0) {
			int32_t i;
			for (i = 0; i <= 32; ++i)
				new_wp("test");
			free_wp_no(0);
			free_wp_no(31);
			new_wp("test2");
			wp_display();
			for (i = 0; i <= 32; ++i)
				free_wp_no(i);
			wp_display();
		}
	}	
	return 0;
}
static int cmd_q(char *args) {
  return -1;
}
static int cmd_help(char *args);
static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "si", "Execution one step", cmd_si },
	{ "i", "Display info about", cmd_i}, 
	{ "x", "Scan memory", cmd_x},
	{ "reg", "ISA_REG_STR2VAL", cmd_reg},
	{ "expr", "Calculate expression", cmd_expr},
	{ "wp", "Set a watch point", cmd_wp},
	{ "dwp", "Remove a watch point", cmd_dwp},
	{ "test", "Test only", cmd_test},
  { "q", "Exit NEMU", cmd_q },


};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);
    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd);  }
	}
}
