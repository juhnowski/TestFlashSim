// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vzns_fsm_validator__pch.h"

//============================================================
// Constructors

Vzns_fsm_validator::Vzns_fsm_validator(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vzns_fsm_validator__Syms(contextp(), _vcname__, this)}
    , clk{vlSymsp->TOP.clk}
    , rst{vlSymsp->TOP.rst}
    , io_trigger{vlSymsp->TOP.io_trigger}
    , io_cmd{vlSymsp->TOP.io_cmd}
    , addr_bound_error{vlSymsp->TOP.addr_bound_error}
    , thermal_shutdown_tripped{vlSymsp->TOP.thermal_shutdown_tripped}
    , bram_we{vlSymsp->TOP.bram_we}
    , out_status{vlSymsp->TOP.out_status}
    , out_err_code{vlSymsp->TOP.out_err_code}
    , validated_zone_id{vlSymsp->TOP.validated_zone_id}
    , validated_target_page{vlSymsp->TOP.validated_target_page}
    , bram_addr{vlSymsp->TOP.bram_addr}
    , bram_rdata{vlSymsp->TOP.bram_rdata}
    , bram_wdata{vlSymsp->TOP.bram_wdata}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
}

Vzns_fsm_validator::Vzns_fsm_validator(const char* _vcname__)
    : Vzns_fsm_validator(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vzns_fsm_validator::~Vzns_fsm_validator() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vzns_fsm_validator___024root___eval_debug_assertions(Vzns_fsm_validator___024root* vlSelf);
#endif  // VL_DEBUG
void Vzns_fsm_validator___024root___eval_static(Vzns_fsm_validator___024root* vlSelf);
void Vzns_fsm_validator___024root___eval_initial(Vzns_fsm_validator___024root* vlSelf);
void Vzns_fsm_validator___024root___eval_settle(Vzns_fsm_validator___024root* vlSelf);
void Vzns_fsm_validator___024root___eval(Vzns_fsm_validator___024root* vlSelf);

void Vzns_fsm_validator::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vzns_fsm_validator::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vzns_fsm_validator___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vzns_fsm_validator___024root___eval_static(&(vlSymsp->TOP));
        Vzns_fsm_validator___024root___eval_initial(&(vlSymsp->TOP));
        Vzns_fsm_validator___024root___eval_settle(&(vlSymsp->TOP));
        vlSymsp->__Vm_didInit = true;
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vzns_fsm_validator___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vzns_fsm_validator::eventsPending() { return false; }

uint64_t Vzns_fsm_validator::nextTimeSlot() {
    VL_FATAL_MT(__FILE__, __LINE__, "", "No delays in the design");
    return 0;
}

//============================================================
// Utilities

const char* Vzns_fsm_validator::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vzns_fsm_validator___024root___eval_final(Vzns_fsm_validator___024root* vlSelf);

VL_ATTR_COLD void Vzns_fsm_validator::final() {
    contextp()->executingFinal(true);
    Vzns_fsm_validator___024root___eval_final(&(vlSymsp->TOP));
    contextp()->executingFinal(false);
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vzns_fsm_validator::hierName() const { return vlSymsp->name(); }
const char* Vzns_fsm_validator::modelName() const { return "Vzns_fsm_validator"; }
unsigned Vzns_fsm_validator::threads() const { return 1; }
void Vzns_fsm_validator::prepareClone() const { contextp()->prepareClone(); }
void Vzns_fsm_validator::atClone() const {
    contextp()->threadPoolpOnClone();
}
