#include "sdb.h"
#include <common.h>
#include <isa.h>
#include <cpu/cpu.h>
#include <memory/host.h>
#include <memory/paddr.h>

#include <stdlib.h>

__attribute__((unused)) int cmd_si(char *args)
{
    char *arg = strtok(NULL, " ");
    if (arg == NULL)
    {
        Log("You must Input the parameter number");
        cpu_exec(1);
        return 0;
    }
    uint64_t number = atoi(arg);
    Log("Step number: %lu", number);
    cpu_exec(number);
    return 0;
}
__attribute__((unused)) int cmd_info(char *args)
{
    char *arg = strtok(NULL, " ");
    if (arg == NULL)
    {
        Log("You must Input the parameter number");
        return 1;
    }
    if (strcmp(arg, "r") == 0)
    {
        isa_reg_display();
        return 0;
    }
    else if (strcmp(arg, "w") == 0)
    {
        show_wp_pool();
        return 0;
    }
    return 0;
}
__attribute__((unused)) int cmd_x(char *args)
{
    // 扫描内存
    char *str_n = strtok(NULL, " ");
    if (str_n == NULL)
    {
        Log("You must Input the parameter number");
        return 0;
    }
    args = args + strlen(str_n) + 1;
    int n = atoi(str_n);
    bool success;
    word_t result = expr(args, &success);
    show_memery(n, result);
    return 0;
}
__attribute__((unused)) int cmd_p(char *args)
{
    // char *arg = strtok(NULL, " ");
    bool success;

    word_t result = expr(args, &success);
    printf("表达式结果是：%ld\n", result);
    return 0;
}
__attribute__((unused)) int cmd_w(char *args)
{
    return 0;
}
__attribute__((unused)) int cmd_d(char *args)
{
    return 0;
}

void show_memery(int n, word_t start)
{
    word_t address = start;
    for (int i = 0; i < n; i++)
    {
        address += i * 4;
        word_t val = host_read(guest_to_host(address), 4);
        printf("Guest Address [%lx] value [%lx] \n", address, val);
    }
    return;
}
