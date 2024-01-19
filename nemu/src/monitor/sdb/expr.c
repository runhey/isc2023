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
#include "sdb.h"
#include <memory/paddr.h>
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
    {"^0x[0-9a-fA-F]{4}", TK_ADDRESS},
    {"^\\$[a-z0-9]+", TK_REG},
    {"^[0-9]+", TK_NUMBER},

};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

void add_token(int type, char *string, int n);
word_t eval(int p, int q);
bool check_parentheses(int p, int q);
int find_op(int p, int q);
word_t decpointer(word_t);

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
        // char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        // Log("match rules[%2d] = \"%-16s\" at position %d with len %d: %.*s",
        //     i, rules[i].regex, position, substr_len, substr_len, substr_start);

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
    printf("tokens[%2d]=> type: %-20s, string: %-32s\n", i, class, tokens[i].str);
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
  for (int i = 0; i < nr_token; i++)
  {
    if (i == 0 && tokens[i].type == TK_STAR)
    {
      tokens[i].type = TK_POINT;
      continue;
    }
    if (tokens[i].type == TK_STAR)
    {
      switch (tokens[i - 1].type)
      {
      case TK_PLUS:
      case TK_SUB:
      case TK_STAR:
      case TK_DIVIDE:
      case TK_EQ:
      case TK_UNEQUAL:
      case TK_AND:
      {
        tokens[i].type = TK_POINT;
      }
      break;

      default:
        break;
      }
    }
  }

  *success = true;
  word_t result = eval(0, nr_token - 1);
  printf("表达式[%s]的值是: ox%lx \n", e, result);
  return result;
}

void add_token(int type, char *string, int n)
{
  tokens[nr_token].type = type;
  strncpy(tokens[nr_token].str, string, n);
  nr_token++;
  return;
}

word_t eval(int p, int q)
{
  if (p > q)
  {
    Log("The p[%d] and q[%d] is invalid", p, q);
    assert(0);
  }
  else if (p == q)
  {
    // 直接返回值就可以了
    switch (tokens[p].type)
    {
    case TK_NUMBER:
    case TK_ADDRESS:
    {
      return strtoul(tokens[p].str, NULL, 0);
    }
    break;
    case TK_REG:
    {
      bool success;
      word_t result = isa_reg_str2val(tokens[p].str, &success);
      return result;
    }
    break;

    default:
    {
      Log("eval tokens[%d] is invalid", p);
      assert(0);
    }
    break;
    }
  }
  else if (check_parentheses(p, q) == true)
  {
    return eval(p + 1, q - 1);
  }
  // else if(p+1 == q){ // 双目运算符
  //   if(tokens[i].type != TK_POINT){
  //     printf("You had input invliad *");
  //     assert(0);
  //   }

  // }
  else
  {
    //--------------------------------------------------------------------
    // 这种情况就是有三个及以上的token
    int op = find_op(p, q);
    Log("Find the main op: %d, p:%d, q:%d", op, p, q);
    // 对于 解指针运算符特殊处理
    if (tokens[op].type == TK_POINT)
    {
      return decpointer(eval(op + 1, q));
    }

    word_t val1 = eval(p, op - 1);
    word_t val2 = eval(op + 1, q);
    switch (tokens[op].type)
    {
    case TK_PLUS: // +
    {
      return val1 + val2;
    }
    break;
    case TK_SUB: // -
    {
      return val1 - val2;
    }
    break;
    case TK_STAR: // *
    {
      return val1 * val2;
    }
    break;
    case TK_DIVIDE: // /
    {
      if (val2 == 0)
      {
        Log("The denominator of division is zero");
        assert(0);
      }
      return val1 / val2;
    }
    break;
    case TK_EQ: // ==
    {
      return val1 == val2;
    }
    break;
    case TK_UNEQUAL: // !=
    {
      return val1 != val2;
    }
    break;
    case TK_AND: // &&
    {
      return val1 && val2;
    }
    break;
    default:
    {
      Log("The correct two-sided operator was not found");
      assert(0);
    }
    break;
    }
  }
}

bool check_parentheses(int p, int q)
{
  // 用栈来表示
  int match_number = 0;
  if (tokens[p].type != TK_LEFT_BRACKETS)
  {
    return false;
  }
  if (tokens[q].type != TK_RIGHT_BRACKETS)
  {
    return false;
  }
  for (int i = p; i < q; i++)
  {
    if (tokens[i].type == TK_LEFT_BRACKETS)
    {
      match_number++;
    }
    if (tokens[i].type == TK_LEFT_BRACKETS)
    {
      match_number--;
    }
    if (match_number == 0 && i == p)
    {
      continue;
    }
    if (match_number < 0)
    {
      return false;
    }
  }
  return true;
}

int find_op(int p, int q)
{
  int brackets_stack = 0;
  for (int i = p; i < q; i++)
  {
    if (tokens[i].type == TK_LEFT_BRACKETS)
    {
      brackets_stack++;
    }
    else if (tokens[i].type == TK_RIGHT_BRACKETS)
    {
      brackets_stack--;
    }
    else if (brackets_stack == 0 && tokens[i].type == TK_PLUS)
    {
      return i;
    }
    else if (brackets_stack == 0 && tokens[i].type == TK_SUB)
    {
      return i;
    }
  }
  brackets_stack = 0;
  for (int i = p; i < q; i++)
  {
    if (tokens[i].type == TK_LEFT_BRACKETS)
    {
      brackets_stack++;
    }
    else if (tokens[i].type == TK_RIGHT_BRACKETS)
    {
      brackets_stack--;
    }
    else if (brackets_stack == 0 && tokens[i].type == TK_STAR)
    {
      return i;
    }
    else if (brackets_stack == 0 && tokens[i].type == TK_DIVIDE)
    {
      return i;
    }
  }
  brackets_stack = 0;
  for (int i = p; i < q; i++)
  {
    if (tokens[i].type == TK_LEFT_BRACKETS)
    {
      brackets_stack++;
    }
    else if (tokens[i].type == TK_RIGHT_BRACKETS)
    {
      brackets_stack--;
    }
    else if (brackets_stack == 0 && tokens[i].type == TK_POINT)
    {
      return i;
    }
  }
  return q;
}

word_t decpointer(word_t start)
{
  paddr_t address = (paddr_t)start + CONFIG_MBASE;
  paddr_t val = *(paddr_t *)guest_to_host(address);
  printf("Guest Address [%x] value [%x] \n", address, val);
  return (word_t)val;
}
