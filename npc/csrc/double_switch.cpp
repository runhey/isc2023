
// Include common routines
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
// Include model header, generated from Verilating "top.v"
#include "Vtop.h"
#include "verilated_vcd_c.h"
#include <verilated.h>
#include <nvboard.h>

void nvboard_bind_all_pins(Vtop *top);

int main(int argc, char **argv)
{
    // See a similar example walkthrough in the verilator manpage.

    // This is intended to be a minimal example.  Before copying this to start a
    // real project, it is better to start with a more complete example,
    // e.g. examples/c_tracing.

    Verilated::mkdir("logs");

    // Construct a VerilatedContext to hold simulation time, etc.
    const std::unique_ptr<VerilatedContext> contextp{new VerilatedContext};

    // Pass arguments so Verilated code can see them, e.g. $value$plusargs
    // This needs to be called before you create any model
    contextp->commandArgs(argc, argv);
    contextp->traceEverOn(true);
    contextp->randReset(2);
    contextp->debug(0);

    // Construct the Verilated model, from Vtop.h generated from Verilating "top.v"

    const std::unique_ptr<Vtop> top{new Vtop{contextp.get(), "TOP"}};
    const std::unique_ptr<VerilatedVcdC> tfp{new VerilatedVcdC()};
    top->trace(tfp.get(), 0);
    tfp->open("logs/wave.vcd");
    nvboard_bind_all_pins(top.get());
    nvboard_init();

    // Simulate until $finish
    // !contextp->gotFinish()
    while (!contextp->gotFinish())
    {
        // int a = rand() & 1;
        // int b = rand() & 1;
        // top->a = a;
        // top->b = b;

        nvboard_update();
        top->eval();
        tfp->dump(contextp->time());
        contextp->timeInc(1);
        printf("[%" PRId64 "] | a = %d, b = %d, f = %d\n", contextp->time(), top->a, top->b, top->f);
        assert(top->f == (top->a ^ top->b));
    }

    // Final model cleanup
    top->final();
    tfp->close();

#if VM_COVERAGE
    Verilated::mkdir("logs");
    contextp->coveragep()->write("logs/coverage.dat");
#endif

    // Return good completion status
    return 0;
}