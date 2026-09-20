// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table internal header
//
// Internal details; most calling programs do not need this header,
// unless using verilator public meta comments.

#ifndef VERILATED_VZNS_FSM_VALIDATOR__SYMS_H_
#define VERILATED_VZNS_FSM_VALIDATOR__SYMS_H_  // guard

#include "verilated.h"

// INCLUDE MODEL CLASS

#include "Vzns_fsm_validator.h"

// INCLUDE MODULE CLASSES
#include "Vzns_fsm_validator___024root.h"

// SYMS CLASS (contains all model state)
class alignas(VL_CACHE_LINE_BYTES) Vzns_fsm_validator__Syms final : public VerilatedSyms {
  public:
    // INTERNAL STATE
    Vzns_fsm_validator* const __Vm_modelp;
    VlDeleter __Vm_deleter;
    bool __Vm_didInit = false;

    // MODULE INSTANCE STATE
    Vzns_fsm_validator___024root   TOP;

    // CONSTRUCTORS
    Vzns_fsm_validator__Syms(VerilatedContext* contextp, const char* namep, Vzns_fsm_validator* modelp);
    ~Vzns_fsm_validator__Syms();

    // METHODS
    const char* name() const { return TOP.vlNamep; }
};

#endif  // guard
