// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vzns_fsm_validator.h for the primary calling header

#include "Vzns_fsm_validator__pch.h"

void Vzns_fsm_validator___024root___ctor_var_reset(Vzns_fsm_validator___024root* vlSelf);

Vzns_fsm_validator___024root::Vzns_fsm_validator___024root(Vzns_fsm_validator__Syms* symsp, const char* namep)
 {
    vlSymsp = symsp;
    vlNamep = strdup(namep);
    // Reset structure values
    Vzns_fsm_validator___024root___ctor_var_reset(this);
}

void Vzns_fsm_validator___024root::__Vconfigure(bool first) {
    (void)first;  // Prevent unused variable warning
}

Vzns_fsm_validator___024root::~Vzns_fsm_validator___024root() {
    VL_DO_DANGLING(std::free(const_cast<char*>(vlNamep)), vlNamep);
}
