#include "sdb.h"
#include <common.h>
#include <isa.h>
#include <cpu/cpu.h>

#include <stdlib.h>

__attribute__((unused)) static int cmd_si(char *args)
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
__attribute__((unused)) static int cmd_info(char *args)
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
__attribute__((unused)) static int cmd_x(char *args)
{
    return 0;
}
__attribute__((unused)) static int cmd_p(char *args)
{
    // char *arg = strtok(NULL, " ");
    bool success;
    // if (arg == NULL)
    // {
    //     Log("You must Input the parameter number");
    //     return 1;
    // }

    // printf("Input arg: %s\n", args);
    // while (arg != NULL)
    // {
    //     expr(arg, &success);
    //     char *arg = strtok(NULL, " ");
    // }

    __attribute__((unused)) word_t result = expr(args, &success);
    return 0;
}
__attribute__((unused)) static int cmd_w(char *args)
{
    return 0;
}
__attribute__((unused)) static int cmd_d(char *args)
{
    return 0;
}
