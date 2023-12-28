
// Include common routines

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
// Include model header, generated from Verilating "top.v"
#include "Vtop.h"
#include "verilated_vcd_c.h"
#include <verilated.h>

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
    tfp->open("wave.vcd");

    // Simulate until $finish
    while (!contextp->gotFinish())
    {

        int a = rand() & 1;
        int b = rand() & 1;
        top->a = a;
        top->b = b;

        top->eval();
        tfp->dump(contextp->time());
        contextp->timeInc(1);
        printf("[%" PRId64 "] | a = %d, b = %d, f = %d\n", contextp->time(), a, b, top->f);
        assert(top->f == (a ^ b));
    }

    // Final model cleanup
    top->final();

#if VM_COVERAGE
    Verilated::mkdir("logs");
    contextp->coveragep()->write("logs/coverage.dat");
#endif

    // Return good completion status
    return 0;
}