// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vzns_fsm_validator.h for the primary calling header

#include "Vzns_fsm_validator__pch.h"

bool Vzns_fsm_validator___024root___trigger_anySet__ico(const VlUnpacked<QData/*63:0*/, 2> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vzns_fsm_validator___024root___trigger_anySet__ico\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        if (in[n]) {
            return (1U);
        }
        n = ((IData)(1U) + n);
    } while ((2U > n));
    return (0U);
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vzns_fsm_validator___024root___dump_triggers__ico(const VlUnpacked<QData/*63:0*/, 2> &triggers, const std::string &tag);
#endif  // VL_DEBUG

bool Vzns_fsm_validator___024root___eval_phase__ico(Vzns_fsm_validator___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vzns_fsm_validator___024root___eval_phase__ico\n"); );
    Vzns_fsm_validator__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VicoExecute;
    // Body
    {
        // Inlined CFunc: _eval_triggers_vec__ico
        vlSelfRef.__VicoTriggered[0U] = (QData)((IData)(
                                                        (((vlSelfRef.bram_rdata 
                                                           != vlSelfRef.__Vtrigprevexpr___TOP__bram_rdata__0) 
                                                          << 8U) 
                                                         | (((((((IData)(vlSelfRef.thermal_shutdown_tripped) 
                                                                 != (IData)(vlSelfRef.__Vtrigprevexpr___TOP__thermal_shutdown_tripped__0)) 
                                                                << 3U) 
                                                               | (((IData)(vlSelfRef.addr_bound_error) 
                                                                   != (IData)(vlSelfRef.__Vtrigprevexpr___TOP__addr_bound_error__0)) 
                                                                  << 2U)) 
                                                              | (((vlSelfRef.validated_target_page 
                                                                   != vlSelfRef.__Vtrigprevexpr___TOP__validated_target_page__0) 
                                                                  << 1U) 
                                                                 | (vlSelfRef.validated_zone_id 
                                                                    != vlSelfRef.__Vtrigprevexpr___TOP__validated_zone_id__0))) 
                                                             << 4U) 
                                                            | (((((IData)(vlSelfRef.io_cmd) 
                                                                  != (IData)(vlSelfRef.__Vtrigprevexpr___TOP__io_cmd__0)) 
                                                                 << 3U) 
                                                                | (((IData)(vlSelfRef.io_trigger) 
                                                                    != (IData)(vlSelfRef.__Vtrigprevexpr___TOP__io_trigger__0)) 
                                                                   << 2U)) 
                                                               | ((((IData)(vlSelfRef.rst) 
                                                                    != (IData)(vlSelfRef.__Vtrigprevexpr___TOP__rst__0)) 
                                                                   << 1U) 
                                                                  | ((IData)(vlSelfRef.clk) 
                                                                     != (IData)(vlSelfRef.__Vtrigprevexpr___TOP__clk__0))))))));
        vlSelfRef.__Vtrigprevexpr___TOP__clk__0 = vlSelfRef.clk;
        vlSelfRef.__Vtrigprevexpr___TOP__rst__0 = vlSelfRef.rst;
        vlSelfRef.__Vtrigprevexpr___TOP__io_trigger__0 
            = vlSelfRef.io_trigger;
        vlSelfRef.__Vtrigprevexpr___TOP__io_cmd__0 
            = vlSelfRef.io_cmd;
        vlSelfRef.__Vtrigprevexpr___TOP__validated_zone_id__0 
            = vlSelfRef.validated_zone_id;
        vlSelfRef.__Vtrigprevexpr___TOP__validated_target_page__0 
            = vlSelfRef.validated_target_page;
        vlSelfRef.__Vtrigprevexpr___TOP__addr_bound_error__0 
            = vlSelfRef.addr_bound_error;
        vlSelfRef.__Vtrigprevexpr___TOP__thermal_shutdown_tripped__0 
            = vlSelfRef.thermal_shutdown_tripped;
        vlSelfRef.__Vtrigprevexpr___TOP__bram_rdata__0 
            = vlSelfRef.bram_rdata;
        if (VL_UNLIKELY(((1U & (~ (IData)(vlSelfRef.__VicoDidInit)))))) {
            vlSelfRef.__VicoDidInit = 1U;
            vlSelfRef.__VicoTriggered[0U] = (1ULL | vlSelfRef.__VicoTriggered[0U]);
            vlSelfRef.__VicoTriggered[0U] = (2ULL | vlSelfRef.__VicoTriggered[0U]);
            vlSelfRef.__VicoTriggered[0U] = (4ULL | vlSelfRef.__VicoTriggered[0U]);
            vlSelfRef.__VicoTriggered[0U] = (8ULL | vlSelfRef.__VicoTriggered[0U]);
            vlSelfRef.__VicoTriggered[0U] = (0x0000000000000010ULL 
                                             | vlSelfRef.__VicoTriggered[0U]);
            vlSelfRef.__VicoTriggered[0U] = (0x0000000000000020ULL 
                                             | vlSelfRef.__VicoTriggered[0U]);
            vlSelfRef.__VicoTriggered[0U] = (0x0000000000000040ULL 
                                             | vlSelfRef.__VicoTriggered[0U]);
            vlSelfRef.__VicoTriggered[0U] = (0x0000000000000080ULL 
                                             | vlSelfRef.__VicoTriggered[0U]);
            vlSelfRef.__VicoTriggered[0U] = (0x0000000000000100ULL 
                                             | vlSelfRef.__VicoTriggered[0U]);
        }
    }
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vzns_fsm_validator___024root___dump_triggers__ico(vlSelfRef.__VicoTriggered, "ico"s);
    }
#endif
    __VicoExecute = Vzns_fsm_validator___024root___trigger_anySet__ico(vlSelfRef.__VicoTriggered);
    if (__VicoExecute) {
        {
            // Inlined CFunc: _eval_ico
            if ((0x0000000000000014ULL & vlSelfRef.__VicoTriggered[0U])) {
                {
                    // Inlined CFunc: _ico_comb__TOP__0
                    vlSelfRef.bram_addr = (vlSelfRef.validated_zone_id 
                                           & (- (IData)(
                                                        ((IData)(vlSelfRef.io_trigger) 
                                                         | (0U 
                                                            != (IData)(vlSelfRef.zns_fsm_validator__DOT__current_state))))));
                }
            }
            if ((0x0000000000000108ULL & vlSelfRef.__VicoTriggered[0U])) {
                {
                    // Inlined CFunc: _ico_comb__TOP__1
                    vlSelfRef.zns_fsm_validator__DOT__is_opening_new_zone 
                        = (IData)(((1U == (IData)(vlSelfRef.io_cmd)) 
                                   & (0U == (0x000000ffU 
                                             & vlSelfRef.bram_rdata))));
                }
            }
            if ((0x00000000000001e8ULL & vlSelfRef.__VicoTriggered[0U])) {
                {
                    // Inlined CFunc: _ico_comb__TOP__2
                    vlSelfRef.zns_fsm_validator__DOT__has_error 
                        = ((IData)(vlSelfRef.thermal_shutdown_tripped) 
                           || ((0x63U == (IData)(vlSelfRef.io_cmd)) 
                               || (((1U == (IData)(vlSelfRef.io_cmd)) 
                                    & (vlSelfRef.bram_rdata 
                                       >> 7U)) || (
                                                   ((1U 
                                                     == (IData)(vlSelfRef.io_cmd)) 
                                                    & (vlSelfRef.validated_target_page 
                                                       != 
                                                       (0x0000007fU 
                                                        & vlSelfRef.bram_rdata))) 
                                                   || ((((1U 
                                                          == (IData)(vlSelfRef.io_cmd)) 
                                                         & (IData)(vlSelfRef.zns_fsm_validator__DOT__is_opening_new_zone)) 
                                                        & (4U 
                                                           <= (IData)(vlSelfRef.zns_fsm_validator__DOT__open_zones_count))) 
                                                       || (((0U 
                                                             == (IData)(vlSelfRef.io_cmd)) 
                                                            & (vlSelfRef.validated_target_page 
                                                               >= 
                                                               (0x0000007fU 
                                                                & vlSelfRef.bram_rdata))) 
                                                           || (((2U 
                                                                 == (IData)(vlSelfRef.io_cmd)) 
                                                                & (0x0aU 
                                                                   <= 
                                                                   (0x000000ffU 
                                                                    & (vlSelfRef.bram_rdata 
                                                                       >> 0x00000010U)))) 
                                                               || (IData)(vlSelfRef.addr_bound_error))))))));
                    vlSelfRef.zns_fsm_validator__DOT__internal_error_code 
                        = ((IData)(vlSelfRef.thermal_shutdown_tripped)
                            ? 8U : ((0x63U == (IData)(vlSelfRef.io_cmd))
                                     ? 6U : (((1U == (IData)(vlSelfRef.io_cmd)) 
                                              & (vlSelfRef.bram_rdata 
                                                 >> 7U))
                                              ? 7U : 
                                             (((1U 
                                                == (IData)(vlSelfRef.io_cmd)) 
                                               & (vlSelfRef.validated_target_page 
                                                  != 
                                                  (0x0000007fU 
                                                   & vlSelfRef.bram_rdata)))
                                               ? 1U
                                               : ((
                                                   ((1U 
                                                     == (IData)(vlSelfRef.io_cmd)) 
                                                    & (IData)(vlSelfRef.zns_fsm_validator__DOT__is_opening_new_zone)) 
                                                   & (4U 
                                                      <= (IData)(vlSelfRef.zns_fsm_validator__DOT__open_zones_count)))
                                                   ? 2U
                                                   : 
                                                  (((0U 
                                                     == (IData)(vlSelfRef.io_cmd)) 
                                                    & (vlSelfRef.validated_target_page 
                                                       >= 
                                                       (0x0000007fU 
                                                        & vlSelfRef.bram_rdata)))
                                                    ? 3U
                                                    : 
                                                   (((2U 
                                                      == (IData)(vlSelfRef.io_cmd)) 
                                                     & (0x0aU 
                                                        <= 
                                                        (0x000000ffU 
                                                         & (vlSelfRef.bram_rdata 
                                                            >> 0x00000010U))))
                                                     ? 5U
                                                     : 
                                                    ((IData)(vlSelfRef.addr_bound_error)
                                                      ? 7U
                                                      : 0U))))))));
                }
            }
        }
    }
    return (__VicoExecute);
}

bool Vzns_fsm_validator___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vzns_fsm_validator___024root___trigger_anySet__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        if (in[n]) {
            return (1U);
        }
        n = ((IData)(1U) + n);
    } while ((1U > n));
    return (0U);
}

void Vzns_fsm_validator___024root___nba_sequent__TOP__1(Vzns_fsm_validator___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vzns_fsm_validator___024root___nba_sequent__TOP__1\n"); );
    Vzns_fsm_validator__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*3:0*/ __Vdly__zns_fsm_validator__DOT__open_zones_count;
    __Vdly__zns_fsm_validator__DOT__open_zones_count = 0;
    // Body
    __Vdly__zns_fsm_validator__DOT__open_zones_count 
        = vlSelfRef.zns_fsm_validator__DOT__open_zones_count;
    if (vlSelfRef.rst) {
        __Vdly__zns_fsm_validator__DOT__open_zones_count = 0U;
    } else if ((4U == (IData)(vlSelfRef.zns_fsm_validator__DOT__current_state))) {
        if ((IData)(((1U == (IData)(vlSelfRef.io_cmd)) 
                     & (0U == (0x000000ffU & vlSelfRef.bram_rdata))))) {
            __Vdly__zns_fsm_validator__DOT__open_zones_count 
                = (0x0000000fU & ((IData)(1U) + (IData)(vlSelfRef.zns_fsm_validator__DOT__open_zones_count)));
        }
        if ((((1U == (IData)(vlSelfRef.io_cmd)) & (0x3fU 
                                                   == 
                                                   (0x0000007fU 
                                                    & vlSelfRef.bram_rdata))) 
             & (0U < (IData)(vlSelfRef.zns_fsm_validator__DOT__open_zones_count)))) {
            __Vdly__zns_fsm_validator__DOT__open_zones_count 
                = (0x0000000fU & ((IData)(vlSelfRef.zns_fsm_validator__DOT__open_zones_count) 
                                  - (IData)(1U)));
        } else if (((((2U == (IData)(vlSelfRef.io_cmd)) 
                      & (0U < (0x0000007fU & vlSelfRef.bram_rdata))) 
                     & (~ (vlSelfRef.bram_rdata >> 7U))) 
                    & (0U < (IData)(vlSelfRef.zns_fsm_validator__DOT__open_zones_count)))) {
            __Vdly__zns_fsm_validator__DOT__open_zones_count 
                = (0x0000000fU & ((IData)(vlSelfRef.zns_fsm_validator__DOT__open_zones_count) 
                                  - (IData)(1U)));
        }
    }
    vlSelfRef.zns_fsm_validator__DOT__open_zones_count 
        = __Vdly__zns_fsm_validator__DOT__open_zones_count;
    vlSelfRef.zns_fsm_validator__DOT__has_error = ((IData)(vlSelfRef.thermal_shutdown_tripped) 
                                                   || ((0x63U 
                                                        == (IData)(vlSelfRef.io_cmd)) 
                                                       || (((1U 
                                                             == (IData)(vlSelfRef.io_cmd)) 
                                                            & (vlSelfRef.bram_rdata 
                                                               >> 7U)) 
                                                           || (((1U 
                                                                 == (IData)(vlSelfRef.io_cmd)) 
                                                                & (vlSelfRef.validated_target_page 
                                                                   != 
                                                                   (0x0000007fU 
                                                                    & vlSelfRef.bram_rdata))) 
                                                               || ((((1U 
                                                                      == (IData)(vlSelfRef.io_cmd)) 
                                                                     & (IData)(vlSelfRef.zns_fsm_validator__DOT__is_opening_new_zone)) 
                                                                    & (4U 
                                                                       <= (IData)(vlSelfRef.zns_fsm_validator__DOT__open_zones_count))) 
                                                                   || (((0U 
                                                                         == (IData)(vlSelfRef.io_cmd)) 
                                                                        & (vlSelfRef.validated_target_page 
                                                                           >= 
                                                                           (0x0000007fU 
                                                                            & vlSelfRef.bram_rdata))) 
                                                                       || (((2U 
                                                                             == (IData)(vlSelfRef.io_cmd)) 
                                                                            & (0x0aU 
                                                                               <= 
                                                                               (0x000000ffU 
                                                                                & (vlSelfRef.bram_rdata 
                                                                                >> 0x00000010U)))) 
                                                                           || (IData)(vlSelfRef.addr_bound_error))))))));
    vlSelfRef.zns_fsm_validator__DOT__internal_error_code 
        = ((IData)(vlSelfRef.thermal_shutdown_tripped)
            ? 8U : ((0x63U == (IData)(vlSelfRef.io_cmd))
                     ? 6U : (((1U == (IData)(vlSelfRef.io_cmd)) 
                              & (vlSelfRef.bram_rdata 
                                 >> 7U)) ? 7U : (((1U 
                                                   == (IData)(vlSelfRef.io_cmd)) 
                                                  & (vlSelfRef.validated_target_page 
                                                     != 
                                                     (0x0000007fU 
                                                      & vlSelfRef.bram_rdata)))
                                                  ? 1U
                                                  : 
                                                 ((((1U 
                                                     == (IData)(vlSelfRef.io_cmd)) 
                                                    & (IData)(vlSelfRef.zns_fsm_validator__DOT__is_opening_new_zone)) 
                                                   & (4U 
                                                      <= (IData)(vlSelfRef.zns_fsm_validator__DOT__open_zones_count)))
                                                   ? 2U
                                                   : 
                                                  (((0U 
                                                     == (IData)(vlSelfRef.io_cmd)) 
                                                    & (vlSelfRef.validated_target_page 
                                                       >= 
                                                       (0x0000007fU 
                                                        & vlSelfRef.bram_rdata)))
                                                    ? 3U
                                                    : 
                                                   (((2U 
                                                      == (IData)(vlSelfRef.io_cmd)) 
                                                     & (0x0aU 
                                                        <= 
                                                        (0x000000ffU 
                                                         & (vlSelfRef.bram_rdata 
                                                            >> 0x00000010U))))
                                                     ? 5U
                                                     : 
                                                    ((IData)(vlSelfRef.addr_bound_error)
                                                      ? 7U
                                                      : 0U))))))));
}

void Vzns_fsm_validator___024root___eval_nba(Vzns_fsm_validator___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vzns_fsm_validator___024root___eval_nba\n"); );
    Vzns_fsm_validator__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((3ULL & vlSelfRef.__VnbaTriggered[0U])) {
        {
            // Inlined CFunc: _nba_sequent__TOP__0
            vlSelfRef.__Vdly__zns_fsm_validator__DOT__current_state 
                = vlSelfRef.zns_fsm_validator__DOT__current_state;
            if (vlSelfRef.rst) {
                vlSelfRef.__Vdly__zns_fsm_validator__DOT__current_state = 0U;
                vlSelfRef.out_status = 0U;
                vlSelfRef.bram_we = 0U;
                vlSelfRef.bram_wdata = 0U;
                vlSelfRef.out_err_code = 0U;
            } else {
                vlSelfRef.__Vdly__zns_fsm_validator__DOT__current_state 
                    = ((4U & (IData)(vlSelfRef.zns_fsm_validator__DOT__current_state))
                        ? ((2U & (IData)(vlSelfRef.zns_fsm_validator__DOT__current_state))
                            ? (6U & (- (IData)(((~ (IData)(vlSelfRef.zns_fsm_validator__DOT__current_state)) 
                                                & (IData)(vlSelfRef.io_trigger)))))
                            : ((1U & (IData)(vlSelfRef.zns_fsm_validator__DOT__current_state))
                                ? (5U & (- (IData)((IData)(vlSelfRef.io_trigger))))
                                : 5U)) : ((2U & (IData)(vlSelfRef.zns_fsm_validator__DOT__current_state))
                                           ? ((1U & (IData)(vlSelfRef.zns_fsm_validator__DOT__current_state))
                                               ? ((IData)(vlSelfRef.zns_fsm_validator__DOT__has_error)
                                                   ? 6U
                                                   : 4U)
                                               : 3U)
                                           : ((1U & (IData)(vlSelfRef.zns_fsm_validator__DOT__current_state))
                                               ? 2U
                                               : (1U 
                                                  & (- (IData)((IData)(vlSelfRef.io_trigger)))))));
                if ((0U == (IData)(vlSelfRef.zns_fsm_validator__DOT__current_state))) {
                    vlSelfRef.out_status = 0U;
                    vlSelfRef.bram_we = 0U;
                } else {
                    if ((1U == (IData)(vlSelfRef.zns_fsm_validator__DOT__current_state))) {
                        vlSelfRef.out_status = 2U;
                    } else if ((6U == (IData)(vlSelfRef.zns_fsm_validator__DOT__current_state))) {
                        vlSelfRef.out_status = 1U;
                    } else if ((4U != (IData)(vlSelfRef.zns_fsm_validator__DOT__current_state))) {
                        if ((5U == (IData)(vlSelfRef.zns_fsm_validator__DOT__current_state))) {
                            vlSelfRef.out_status = 3U;
                        }
                    }
                    if ((1U != (IData)(vlSelfRef.zns_fsm_validator__DOT__current_state))) {
                        if ((6U != (IData)(vlSelfRef.zns_fsm_validator__DOT__current_state))) {
                            if ((4U == (IData)(vlSelfRef.zns_fsm_validator__DOT__current_state))) {
                                vlSelfRef.bram_we = 1U;
                            } else if ((5U == (IData)(vlSelfRef.zns_fsm_validator__DOT__current_state))) {
                                vlSelfRef.bram_we = 0U;
                            }
                        }
                    }
                }
                if ((0U != (IData)(vlSelfRef.zns_fsm_validator__DOT__current_state))) {
                    if ((1U != (IData)(vlSelfRef.zns_fsm_validator__DOT__current_state))) {
                        if ((6U != (IData)(vlSelfRef.zns_fsm_validator__DOT__current_state))) {
                            if ((4U == (IData)(vlSelfRef.zns_fsm_validator__DOT__current_state))) {
                                if ((1U == (IData)(vlSelfRef.io_cmd))) {
                                    vlSelfRef.bram_wdata 
                                        = ((0x3fU == 
                                            (0x0000007fU 
                                             & vlSelfRef.bram_rdata))
                                            ? (0x00c0U 
                                               | (0x00ff0000U 
                                                  & vlSelfRef.bram_rdata))
                                            : ((0x00ff0000U 
                                                & vlSelfRef.bram_rdata) 
                                               | (0x0000007fU 
                                                  & ((IData)(1U) 
                                                     + vlSelfRef.bram_rdata))));
                                } else if ((2U == (IData)(vlSelfRef.io_cmd))) {
                                    vlSelfRef.bram_wdata 
                                        = (0x00ff0000U 
                                           & (((IData)(1U) 
                                               + (vlSelfRef.bram_rdata 
                                                  >> 0x00000010U)) 
                                              << 0x00000010U));
                                }
                            }
                        }
                        if ((6U == (IData)(vlSelfRef.zns_fsm_validator__DOT__current_state))) {
                            vlSelfRef.out_err_code 
                                = vlSelfRef.zns_fsm_validator__DOT__internal_error_code;
                        } else if ((4U != (IData)(vlSelfRef.zns_fsm_validator__DOT__current_state))) {
                            if ((5U == (IData)(vlSelfRef.zns_fsm_validator__DOT__current_state))) {
                                vlSelfRef.out_err_code = 0U;
                            }
                        }
                    }
                }
            }
        }
    }
    if ((1ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vzns_fsm_validator___024root___nba_sequent__TOP__1(vlSelf);
    }
    if ((3ULL & vlSelfRef.__VnbaTriggered[0U])) {
        {
            // Inlined CFunc: _nba_sequent__TOP__2
            vlSelfRef.zns_fsm_validator__DOT__current_state 
                = vlSelfRef.__Vdly__zns_fsm_validator__DOT__current_state;
            vlSelfRef.bram_addr = (vlSelfRef.validated_zone_id 
                                   & (- (IData)(((IData)(vlSelfRef.io_trigger) 
                                                 | (0U 
                                                    != (IData)(vlSelfRef.zns_fsm_validator__DOT__current_state))))));
        }
    }
}

void Vzns_fsm_validator___024root___trigger_orInto__act_vec_vec(VlUnpacked<QData/*63:0*/, 1> &out, const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vzns_fsm_validator___024root___trigger_orInto__act_vec_vec\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = (out[n] | in[n]);
        n = ((IData)(1U) + n);
    } while ((0U >= n));
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vzns_fsm_validator___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG

bool Vzns_fsm_validator___024root___eval_phase__act(Vzns_fsm_validator___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vzns_fsm_validator___024root___eval_phase__act\n"); );
    Vzns_fsm_validator__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    {
        // Inlined CFunc: _eval_triggers_vec__act
        vlSelfRef.__VactTriggered[0U] = (QData)((IData)(
                                                        ((((IData)(vlSelfRef.rst) 
                                                           & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__rst__1))) 
                                                          << 1U) 
                                                         | ((IData)(vlSelfRef.clk) 
                                                            & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__clk__1))))));
        vlSelfRef.__Vtrigprevexpr___TOP__clk__1 = vlSelfRef.clk;
        vlSelfRef.__Vtrigprevexpr___TOP__rst__1 = vlSelfRef.rst;
    }
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vzns_fsm_validator___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
    }
#endif
    Vzns_fsm_validator___024root___trigger_orInto__act_vec_vec(vlSelfRef.__VnbaTriggered, vlSelfRef.__VactTriggered);
    return (0U);
}

void Vzns_fsm_validator___024root___trigger_clear__act(VlUnpacked<QData/*63:0*/, 1> &out) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vzns_fsm_validator___024root___trigger_clear__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = 0ULL;
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

bool Vzns_fsm_validator___024root___eval_phase__nba(Vzns_fsm_validator___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vzns_fsm_validator___024root___eval_phase__nba\n"); );
    Vzns_fsm_validator__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = Vzns_fsm_validator___024root___trigger_anySet__act(vlSelfRef.__VnbaTriggered);
    if (__VnbaExecute) {
        Vzns_fsm_validator___024root___eval_nba(vlSelf);
        Vzns_fsm_validator___024root___trigger_clear__act(vlSelfRef.__VnbaTriggered);
    }
    return (__VnbaExecute);
}

void Vzns_fsm_validator___024root___eval(Vzns_fsm_validator___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vzns_fsm_validator___024root___eval\n"); );
    Vzns_fsm_validator__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VicoIterCount;
    IData/*31:0*/ __VnbaIterCount;
    // Body
    __VicoIterCount = 0U;
    do {
        if (VL_UNLIKELY(((0x00002710U < __VicoIterCount)))) {
#ifdef VL_DEBUG
            Vzns_fsm_validator___024root___dump_triggers__ico(vlSelfRef.__VicoTriggered, "ico"s);
#endif
            VL_FATAL_MT("zns_fsm_validator.v", 8, "", "DIDNOTCONVERGE: Input combinational region did not converge after '--converge-limit' of 10000 tries");
        }
        __VicoIterCount = ((IData)(1U) + __VicoIterCount);
        vlSelfRef.__VicoPhaseResult = Vzns_fsm_validator___024root___eval_phase__ico(vlSelf);
    } while (vlSelfRef.__VicoPhaseResult);
    __VnbaIterCount = 0U;
    do {
        if (VL_UNLIKELY(((0x00002710U < __VnbaIterCount)))) {
#ifdef VL_DEBUG
            Vzns_fsm_validator___024root___dump_triggers__act(vlSelfRef.__VnbaTriggered, "nba"s);
#endif
            VL_FATAL_MT("zns_fsm_validator.v", 8, "", "DIDNOTCONVERGE: NBA region did not converge after '--converge-limit' of 10000 tries");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        vlSelfRef.__VactIterCount = 0U;
        do {
            if (VL_UNLIKELY(((0x00002710U < vlSelfRef.__VactIterCount)))) {
#ifdef VL_DEBUG
                Vzns_fsm_validator___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
#endif
                VL_FATAL_MT("zns_fsm_validator.v", 8, "", "DIDNOTCONVERGE: Active region did not converge after '--converge-limit' of 10000 tries");
            }
            vlSelfRef.__VactIterCount = ((IData)(1U) 
                                         + vlSelfRef.__VactIterCount);
            vlSelfRef.__VactPhaseResult = Vzns_fsm_validator___024root___eval_phase__act(vlSelf);
        } while (vlSelfRef.__VactPhaseResult);
        vlSelfRef.__VnbaPhaseResult = Vzns_fsm_validator___024root___eval_phase__nba(vlSelf);
    } while (vlSelfRef.__VnbaPhaseResult);
}

#ifdef VL_DEBUG
void Vzns_fsm_validator___024root___eval_debug_assertions(Vzns_fsm_validator___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vzns_fsm_validator___024root___eval_debug_assertions\n"); );
    Vzns_fsm_validator__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if (VL_UNLIKELY(((vlSelfRef.clk & 0xfeU)))) {
        Verilated::overWidthError("clk");
    }
    if (VL_UNLIKELY(((vlSelfRef.rst & 0xfeU)))) {
        Verilated::overWidthError("rst");
    }
    if (VL_UNLIKELY(((vlSelfRef.io_trigger & 0xfeU)))) {
        Verilated::overWidthError("io_trigger");
    }
    if (VL_UNLIKELY(((vlSelfRef.addr_bound_error & 0xfeU)))) {
        Verilated::overWidthError("addr_bound_error");
    }
    if (VL_UNLIKELY(((vlSelfRef.thermal_shutdown_tripped 
                      & 0xfeU)))) {
        Verilated::overWidthError("thermal_shutdown_tripped");
    }
}
#endif  // VL_DEBUG
