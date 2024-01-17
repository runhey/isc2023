/***************************************************************************************
 * Copyright (c) 2014-2022 Zihao Yu, Nanjing University
 *
 * NEMU is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 *
 * See the Mulan PSL v2 for more details.
 ***************************************************************************************/

#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

#define TOKENT_MAX 32

enum
{
  TK_NOTYPE = 256,
  TK_PLUS,
  TK_SUB,
  TK_TIME,  // 乘法
  TK_POINT, // 指针
  TK_STAR,  // *
  TK_DIVIDE,
  TK_EQ,
  TK_UNEQUAL,
  TK_AND,
  TK_LEFT_BRACKETS, // 左括号
  TK_RIGHT_BRACKETS,
  TK_ADDRESS, //
  TK_REG,     //
  TK_NUMBER,

  /* TODO: Add more token types */

};

static struct rule
{
  const char *regex;
  int token_type;
} rules[] = {

    {" +", TK_NOTYPE}, // spaces
    {"\\+", TK_PLUS},  // plus
    {"-", TK_SUB},
    {"\\*", TK_STAR},
    {"/", TK_DIVIDE},
    {"==", TK_EQ}, // equal
    {"!=", TK_UNEQUAL},
    {"&&", TK_AND},
    {"\\(", TK_LEFT_BRACKETS},
    {"\\)", TK_RIGHT_BRACKETS},
    {"^0x[0-9a-fA-F]{8}", TK_ADDRESS},
    {"^\\$[a-z]+", TK_REG},
    {"^[0-9]+", TK_NUMBER},

};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

void add_token(int type, char *string, int n);

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex()
{
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i++)
  {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0)
    {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token
{
  int type;
  char str[32];
} Token;

static Token tokens[TOKENT_MAX] __attribute__((used)) = {};
static int nr_token __attribute__((used)) = 0;

static bool make_token(char *e)
{
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0')
  {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i++)
    {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0)
      {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type)
        {
        case TK_NOTYPE:
          break;
        //
        // case TK_STAR:
        // {
        //   if(tokens[nr_token-1].type == TK_NUMBER)
        // }
        // break;
        default:
        {
          add_token(rules[i].token_type, e + position, substr_len);
        }
        break;
        }
        position += substr_len;

        break;
      }
    }

    if (i == NR_REGEX)
    {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  for (int i = 0; i < nr_token; i++)
  {
    const char *class;
    for (int j = 0; j < NR_REGEX; j++)
    {
      if (rules[j].token_type == tokens[i].type)
      {
        class = rules[j].regex;
        break;
      }
    }
    printf("tokens[%2d]=> type: %7s, string: %-32s\n", i, class, tokens[i].str);
  }

  return true;
}

word_t expr(char *e, bool *success)
{
  if (!make_token(e))
  {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  // TODO();

  return 0;
}

void add_token(int type, char *string, int n)
{
  tokens[nr_token].type = type;
  strncpy(tokens[nr_token].str, string, n);
  nr_token++;
  return;
}
