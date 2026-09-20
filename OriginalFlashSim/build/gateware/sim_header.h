#ifndef __SIM_CORE_H_
#define __SIM_CORE_H_
#include "pads.h"

struct pad_s sim_trace[] = {
    { (char*)"sim_trace", 1, NULL },
    { NULL, 0, NULL }
};

struct pad_s clk[] = {
    { (char*)"clk", 1, NULL },
    { NULL, 0, NULL }
};

struct pad_s rst[] = {
    { (char*)"rst", 1, NULL },
    { NULL, 0, NULL }
};

#ifndef __cplusplus
void litex_sim_init(void **out);
#endif

#endif /* __SIM_CORE_H_ */
