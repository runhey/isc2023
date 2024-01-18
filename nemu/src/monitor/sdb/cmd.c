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
    word_t raaa = expr(args, &success);
    show_memery(n, raaa);
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
    new_wp(args);
    return 0;
}
__attribute__((unused)) int cmd_d(char *args)
{
    char *str_n = strtok(NULL, " ");
    if (str_n == NULL)
    {
        Log("You must Input the parameter number");
        return 0;
    }
    int number = atoi(str_n);
    free_wp(number);
    return 0;
}

void show_memery(int n, word_t start)
{

    paddr_t address = (paddr_t)start;
    address = address + CONFIG_MBASE;
    for (int i = 0; i < n; i++)
    {
        paddr_t val = *(paddr_t *)guest_to_host(address);
        printf("Guest Address [%x] value [%x] \n", address, val);
        address += 4;
    }
    return;
}
