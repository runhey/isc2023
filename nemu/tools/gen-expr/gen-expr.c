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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
    "#include <stdio.h>\n"
    "int main() { "
    "  unsigned result = %s; "
    "  printf(\"%%u\", result); "
    "  return 0; "
    "}";
#define RAND(m, n) (rand() % (n - m + 1) + m)
int choose(int n)
{
  int rand_numer = RAND(0, n);
  // printf("生成随机数：%d, 原定最大值[%d]\n", rand_numer, n);
  return rand_numer;
}
void gen(char *str)
{
  int buff_len = strlen(buf);
  // printf("现在向长度为[%d]的 [%s] 写入%s\n", buff_len, buf, str);
  strcpy(buf + buff_len, str);
}
void gen_num()
{
  int num = RAND(1, 1000);
  char tmp_char[5] = {};
  sprintf(tmp_char, "%d", num);
  gen(tmp_char);
}
void gen_rand_op()
{
  switch (choose(3))
  {
  case 0:
    gen("+");
    break;
  case 1:
    gen("-");
    break;
  case 2:
    gen("*");
    break;
  case 3:
    gen("/");
    break;
  default:
    break;
  }
  return;
}

static void gen_rand_expr()
{
  switch (choose(3))
  {
  case 0:
    gen_rand_expr();
    gen_rand_op();
    gen_rand_expr();
    break;
  case 1:
    gen("(");
    gen_rand_expr();
    gen(")");
    break;
  default:
    gen_num();
    break;
  }
}

int main(int argc, char *argv[])
{
  printf("----------------------------------------------------------------------\n\n\n");
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1)
  {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i++)
  {
    memset(buf, 0, sizeof(buf));
    memset(code_buf, 0, sizeof(code_buf));
    gen_rand_expr();

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0)
      continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    ret = fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
