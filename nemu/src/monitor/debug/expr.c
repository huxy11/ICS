#include "nemu.h"
#include <stdlib.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_NUM, TK_HEX, TK_REG, TK_EQ, TK_NE, TK_AND, TK_REF, 
};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {
	{"\\s+", TK_NOTYPE}, 
  {"\\+", '+'},         // plus
	{"\\-", '-'},
	{"\\*", '*'},
	{"\\/", '/'},
  {"==", TK_EQ},         // equal
	{"!=", TK_NE},
	{"\\&\\&", TK_AND},
	{"\\(", '('},
	{"\\)", ')'},
	{"\\$[a-z]+", TK_REG},
	{"0[xX][0-9a-fA-F]+", TK_HEX},
	{"[0-9]+", TK_NUM},
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;
#define MAX_TOKENS 65535
static Token tokens[MAX_TOKENS] = {};
static int nr_token = 0;

static bool check_parentheses(uint32_t, uint32_t)__attribute__((used));
static int32_t eval(uint32_t, uint32_t)__attribute__((used));
static inline uint32_t prio(int32_t type)
{
	switch (type) {
		case TK_AND:
			return 125;
		case TK_EQ:
		case TK_NE:
			return 126;
		case '+':
		case '-':
			return 127;
		case '*':
		case '/':
			return 128;
		case TK_REF:
			return 129;
		default:
			return 255;
	}
}

static inline bool is_reference(int32_t nr)
{
	int32_t type = tokens[nr - 1].type;	
	if ((nr > 0) && ((type == ')') || (type <=TK_REG && type >= TK_NUM)))
		return false;
	return true;
}

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; ++i) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;
        //Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
    //        i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;
				if (rules[i].token_type == TK_NOTYPE)
					break;
				tokens[nr_token].type = rules[i].token_type;
				if (tokens[nr_token].type == '*') {
					if (is_reference(nr_token))
						tokens[nr_token].type = TK_REF;
				}
				Assert(substr_len <= 32, "Token too long.");
				//snprintf(tokens[nr_token++].str, substr_len, "%s", substr_start);
				strncpy(tokens[nr_token].str, substr_start, substr_len);
				tokens[nr_token++].str[substr_len] = '\0';

				/*
        switch (rules[i].token_type) {
					case TK_NUM:
						break;
					default:break;
        }
				*/
				break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

uint32_t expr(const char *e, bool *success) {
	char str[128];
	strcpy(str, e);
  if (!make_token(str)) {
    *success = false;
    return 0;
  }
	*success = true;
	/*
	int i = 0;
	printf("nr_token = %d\n", nr_token);
	for (;i < nr_token; ++i) {
		printf("%s", tokens[i].str);
		if (tokens[i].type == TK_REF)
			printf("(REF)");
	}
	printf("\n");
	return 0;
	*/
  return (uint32_t)eval(0, nr_token - 1);
}

static int32_t eval(uint32_t p, uint32_t q){
	Assert(q < MAX_TOKENS && p <= q, "Invalid arguments.p = %d q = %d", p, q);
	if (p == q) {
		Assert(tokens[p].type == TK_NUM || tokens[p].type == TK_HEX \
				|| tokens[p].type == TK_REG, "Type invalid while p == q.");
		switch (tokens[p].type) {
			case TK_NUM:
				return (uint32_t)atoi(tokens[p].str);
			case TK_HEX:
				return (uint32_t)atox(tokens[p].str+2);
			case TK_REG:
				return isa_reg_str2val(tokens[p].str+1);
		}
		if (tokens[p].type == TK_NUM)
			return (uint32_t)atoi(tokens[p].str);
		if (tokens[p].type == TK_HEX)
			return (uint32_t)atox(tokens[p].str+2);

	} else if (check_parentheses(p, q) == true) {
		return eval(p + 1, q - 1);
	} else {
		uint32_t i, pos = p, pos_prio = 255;	
		int32_t flag = 0;
		for (i = p; i <= q; ++i) {
			if (tokens[i].type == '(')
				++flag;	
			else if (tokens[i].type == ')')
				--flag;
			else if (!flag){
				if (pos_prio >= prio(tokens[i].type)) {
					pos_prio = prio(tokens[i].type);
					pos = i;
				}
			}	
		}
		/*
		printf("-----------------\
				\ndominated op:%s\nFirst:", tokens[pos].str );
		int32_t m;
		for (m = p; m < pos; ++m) {
			printf("%s", tokens[m].str);
		}
			printf("\nSecond:");
		for (m = pos + 1; m <= q; ++m) {
			printf("%s", tokens[m].str);
		}
			printf("\n----------------------");
			*/
		switch (tokens[pos].type) {
			case '+': return eval(p, pos-1) + eval(pos+1, q);
			case '-': return eval(p, pos-1) - eval(pos+1, q);
			case '*': return eval(p, pos-1) * eval(pos+1, q);
			case '/': return eval(p, pos-1) / eval(pos+1, q);
			case TK_EQ: return eval(p, pos-1) == eval(pos+1, q);
			case TK_NE: return eval(p, pos-1) != eval(pos+1, q);
			case TK_AND: return eval(p, pos-1) && eval(pos+1, q);
			case TK_REF: return isa_vaddr_read(eval(pos+1, q), 4);
			default: {
								int32_t m = 0;
							 for (m = p; m <=q; ++m)
								 printf("%s ",tokens[m].str);
								panic("invalid token %c:%d", tokens[pos].type, tokens[pos].type);
							 }
		}
	}
	panic("Wrong");
	return 0;
}
static bool check_parentheses(uint32_t l, uint32_t r)
{
	Assert(l < MAX_TOKENS && r < MAX_TOKENS,"Invalid arguments.");
	int32_t i, re = 0;
	for (i = l; i <= r; ++i) {
		if (tokens[i].type == '(')
			re++;
		else if (tokens[i].type == ')')
			re--;
		if (i != r && re <= 0)
			return false;
	}
	if (!re)
	 return true;	
	return false;
}
