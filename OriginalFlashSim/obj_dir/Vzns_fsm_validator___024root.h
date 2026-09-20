// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vzns_fsm_validator.h for the primary calling header

#ifndef VERILATED_VZNS_FSM_VALIDATOR___024ROOT_H_
#define VERILATED_VZNS_FSM_VALIDATOR___024ROOT_H_  // guard

#include "verilated.h"


class Vzns_fsm_validator__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vzns_fsm_validator___024root final {
  public:

    // DESIGN SPECIFIC STATE
    VL_IN8(clk,0,0);
    VL_IN8(rst,0,0);
    VL_IN8(io_trigger,0,0);
    VL_IN8(io_cmd,7,0);
    VL_IN8(addr_bound_error,0,0);
    VL_IN8(thermal_shutdown_tripped,0,0);
    VL_OUT8(bram_we,0,0);
    VL_OUT8(out_status,7,0);
    VL_OUT8(out_err_code,7,0);
    CData/*2:0*/ zns_fsm_validator__DOT__current_state;
    CData/*3:0*/ zns_fsm_validator__DOT__open_zones_count;
    CData/*0:0*/ zns_fsm_validator__DOT__has_error;
    CData/*7:0*/ zns_fsm_validator__DOT__internal_error_code;
    CData/*0:0*/ zns_fsm_validator__DOT__is_opening_new_zone;
    CData/*2:0*/ __Vdly__zns_fsm_validator__DOT__current_state;
    CData/*0:0*/ __VstlFirstIteration;
    CData/*0:0*/ __VstlPhaseResult;
    CData/*0:0*/ __Vtrigprevexpr___TOP__clk__0;
    CData/*0:0*/ __Vtrigprevexpr___TOP__rst__0;
    CData/*0:0*/ __Vtrigprevexpr___TOP__io_trigger__0;
    CData/*7:0*/ __Vtrigprevexpr___TOP__io_cmd__0;
    CData/*0:0*/ __Vtrigprevexpr___TOP__addr_bound_error__0;
    CData/*0:0*/ __Vtrigprevexpr___TOP__thermal_shutdown_tripped__0;
    CData/*0:0*/ __VicoDidInit;
    CData/*0:0*/ __VicoPhaseResult;
    CData/*0:0*/ __Vtrigprevexpr___TOP__clk__1;
    CData/*0:0*/ __Vtrigprevexpr___TOP__rst__1;
    CData/*0:0*/ __VactPhaseResult;
    CData/*0:0*/ __VnbaPhaseResult;
    VL_IN(validated_zone_id,31,0);
    VL_IN(validated_target_page,31,0);
    VL_OUT(bram_addr,31,0);
    VL_IN(bram_rdata,31,0);
    VL_OUT(bram_wdata,31,0);
    IData/*31:0*/ __Vtrigprevexpr___TOP__validated_zone_id__0;
    IData/*31:0*/ __Vtrigprevexpr___TOP__validated_target_page__0;
    IData/*31:0*/ __Vtrigprevexpr___TOP__bram_rdata__0;
    IData/*31:0*/ __VactIterCount;
    VlUnpacked<QData/*63:0*/, 1> __VstlTriggered;
    VlUnpacked<QData/*63:0*/, 2> __VicoTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VactTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VnbaTriggered;

    // INTERNAL VARIABLES
    Vzns_fsm_validator__Syms* vlSymsp;
    const char* vlNamep;

    // CONSTRUCTORS
    Vzns_fsm_validator___024root(Vzns_fsm_validator__Syms* symsp, const char* namep);
    ~Vzns_fsm_validator___024root();
    VL_UNCOPYABLE(Vzns_fsm_validator___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
