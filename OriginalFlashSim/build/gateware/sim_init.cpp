#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Vsim.h"
#include <verilated.h>
#include "sim_header.h"

extern "C" void litex_sim_init_tracer(void *vsim, long start, long end);
extern "C" void litex_sim_tracer_dump();

extern "C" void litex_sim_dump()
{
}

extern "C" void litex_sim_init(void **out)
{
    Vsim *sim;

    sim = new Vsim;

    litex_sim_init_tracer(sim, 0, -1);

    sim_trace[0].signal = &sim->sim_trace;
    litex_sim_register_pads(sim_trace, (char*)"sim_trace", 0);

    clk[0].signal = &sim->clk;
    litex_sim_register_pads(clk, (char*)"clk", 0);

    rst[0].signal = &sim->rst;
    litex_sim_register_pads(rst, (char*)"rst", 0);

    *out=sim;
}
