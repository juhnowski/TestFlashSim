// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vzns_fsm_validator.h for the primary calling header

#include "Vzns_fsm_validator__pch.h"

VL_ATTR_COLD void Vzns_fsm_validator___024root___eval_static(Vzns_fsm_validator___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vzns_fsm_validator___024root___eval_static\n"); );
    Vzns_fsm_validator__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__Vtrigprevexpr___TOP__clk__0 = vlSelfRef.clk;
    vlSelfRef.__Vtrigprevexpr___TOP__rst__0 = vlSelfRef.rst;
    vlSelfRef.__Vtrigprevexpr___TOP__io_trigger__0 
        = vlSelfRef.io_trigger;
    vlSelfRef.__Vtrigprevexpr___TOP__io_cmd__0 = vlSelfRef.io_cmd;
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
    vlSelfRef.__Vtrigprevexpr___TOP__clk__1 = vlSelfRef.clk;
    vlSelfRef.__Vtrigprevexpr___TOP__rst__1 = vlSelfRef.rst;
}

VL_ATTR_COLD void Vzns_fsm_validator___024root___eval_initial(Vzns_fsm_validator___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vzns_fsm_validator___024root___eval_initial\n"); );
    Vzns_fsm_validator__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

VL_ATTR_COLD void Vzns_fsm_validator___024root___eval_final(Vzns_fsm_validator___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vzns_fsm_validator___024root___eval_final\n"); );
    Vzns_fsm_validator__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vzns_fsm_validator___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vzns_fsm_validator___024root___eval_phase__stl(Vzns_fsm_validator___024root* vlSelf);

VL_ATTR_COLD void Vzns_fsm_validator___024root___eval_settle(Vzns_fsm_validator___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vzns_fsm_validator___024root___eval_settle\n"); );
    Vzns_fsm_validator__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VstlIterCount;
    // Body
    __VstlIterCount = 0U;
    vlSelfRef.__VstlFirstIteration = 1U;
    do {
        if (VL_UNLIKELY(((0x00002710U < __VstlIterCount)))) {
#ifdef VL_DEBUG
            Vzns_fsm_validator___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
#endif
            VL_FATAL_MT("zns_fsm_validator.v", 8, "", "DIDNOTCONVERGE: Settle region did not converge after '--converge-limit' of 10000 tries");
        }
        __VstlIterCount = ((IData)(1U) + __VstlIterCount);
        vlSelfRef.__VstlPhaseResult = Vzns_fsm_validator___024root___eval_phase__stl(vlSelf);
        vlSelfRef.__VstlFirstIteration = 0U;
    } while (vlSelfRef.__VstlPhaseResult);
}

VL_ATTR_COLD bool Vzns_fsm_validator___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vzns_fsm_validator___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vzns_fsm_validator___024root___dump_triggers__stl\n"); );
    // Body
    if ((1U & (~ (IData)(Vzns_fsm_validator___024root___trigger_anySet__stl(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: Internal 'stl' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD bool Vzns_fsm_validator___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vzns_fsm_validator___024root___trigger_anySet__stl\n"); );
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

VL_ATTR_COLD bool Vzns_fsm_validator___024root___eval_phase__stl(Vzns_fsm_validator___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vzns_fsm_validator___024root___eval_phase__stl\n"); );
    Vzns_fsm_validator__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VstlExecute;
    // Body
    {
        // Inlined CFunc: _eval_triggers_vec__stl
        vlSelfRef.__VstlTriggered[0U] = ((0xfffffffffffffffeULL 
                                          & vlSelfRef.__VstlTriggered[0U]) 
                                         | (IData)((IData)(vlSelfRef.__VstlFirstIteration)));
    }
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vzns_fsm_validator___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
    }
#endif
    __VstlExecute = Vzns_fsm_validator___024root___trigger_anySet__stl(vlSelfRef.__VstlTriggered);
    if (__VstlExecute) {
        {
            // Inlined CFunc: _eval_stl
            if ((1ULL & vlSelfRef.__VstlTriggered[0U])) {
                {
                    // Inlined CFunc: _stl_sequent__TOP__0
                    vlSelfRef.bram_addr = (vlSelfRef.validated_zone_id 
                                           & (- (IData)(
                                                        ((IData)(vlSelfRef.io_trigger) 
                                                         | ((4U 
                                                             == (IData)(vlSelfRef.zns_fsm_validator__DOT__current_state)) 
                                                            | ((1U 
                                                                == (IData)(vlSelfRef.zns_fsm_validator__DOT__current_state)) 
                                                               | ((2U 
                                                                   == (IData)(vlSelfRef.zns_fsm_validator__DOT__current_state)) 
                                                                  | (5U 
                                                                     == (IData)(vlSelfRef.zns_fsm_validator__DOT__current_state)))))))));
                    vlSelfRef.zns_fsm_validator__DOT__is_opening_new_zone 
                        = (IData)(((1U == (IData)(vlSelfRef.io_cmd)) 
                                   & (0U == (0x000000ffU 
                                             & vlSelfRef.bram_rdata))));
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
                                                                & (3U 
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
                                                     & (3U 
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
    return (__VstlExecute);
}

bool Vzns_fsm_validator___024root___trigger_anySet__ico(const VlUnpacked<QData/*63:0*/, 2> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vzns_fsm_validator___024root___dump_triggers__ico(const VlUnpacked<QData/*63:0*/, 2> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vzns_fsm_validator___024root___dump_triggers__ico\n"); );
    // Body
    if ((1U & (~ (IData)(Vzns_fsm_validator___024root___trigger_anySet__ico(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: @( clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 1U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 1 is active: @( rst)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 2U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 2 is active: @( io_trigger)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 3U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 3 is active: @( io_cmd)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 4U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 4 is active: @( validated_zone_id)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 5U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 5 is active: @( validated_target_page)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 6U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 6 is active: @( addr_bound_error)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 7U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 7 is active: @( thermal_shutdown_tripped)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 8U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 8 is active: @( bram_rdata)\n");
    }
    if ((1U & (IData)(triggers[1U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 64 is active: Internal 'ico' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

bool Vzns_fsm_validator___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vzns_fsm_validator___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vzns_fsm_validator___024root___dump_triggers__act\n"); );
    // Body
    if ((1U & (~ (IData)(Vzns_fsm_validator___024root___trigger_anySet__act(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: @(posedge clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 1U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 1 is active: @(posedge rst)\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vzns_fsm_validator___024root___ctor_var_reset(Vzns_fsm_validator___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vzns_fsm_validator___024root___ctor_var_reset\n"); );
    Vzns_fsm_validator__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->vlNamep);
    vlSelf->clk = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16707436170211756652ull);
    vlSelf->rst = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 18209466448985614591ull);
    vlSelf->io_trigger = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11702515870220541776ull);
    vlSelf->io_cmd = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 8862194297607448292ull);
    vlSelf->validated_zone_id = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 4260644288930188368ull);
    vlSelf->validated_target_page = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 14687996885297042022ull);
    vlSelf->addr_bound_error = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17038217529723612280ull);
    vlSelf->thermal_shutdown_tripped = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 787050669766383031ull);
    vlSelf->bram_addr = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 10458784103138679157ull);
    vlSelf->bram_rdata = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 17664034088567969531ull);
    vlSelf->bram_wdata = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 5145022849444923699ull);
    vlSelf->bram_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5892086736947945850ull);
    vlSelf->out_status = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 15606078872201372946ull);
    vlSelf->out_err_code = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 6154418305893886848ull);
    vlSelf->zns_fsm_validator__DOT__current_state = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 6882111589295431825ull);
    vlSelf->zns_fsm_validator__DOT__open_zones_count = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 1751866187105926349ull);
    vlSelf->zns_fsm_validator__DOT__has_error = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15961367106081493593ull);
    vlSelf->zns_fsm_validator__DOT__internal_error_code = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 13643812718829098627ull);
    vlSelf->zns_fsm_validator__DOT__is_opening_new_zone = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 948012770907508813ull);
    vlSelf->__Vdly__zns_fsm_validator__DOT__current_state = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VstlTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 2; ++__Vi0) {
        vlSelf->__VicoTriggered[__Vi0] = 0;
    }
    vlSelf->__Vtrigprevexpr___TOP__clk__0 = 0;
    vlSelf->__Vtrigprevexpr___TOP__rst__0 = 0;
    vlSelf->__Vtrigprevexpr___TOP__io_trigger__0 = 0;
    vlSelf->__Vtrigprevexpr___TOP__io_cmd__0 = 0;
    vlSelf->__Vtrigprevexpr___TOP__validated_zone_id__0 = 0;
    vlSelf->__Vtrigprevexpr___TOP__validated_target_page__0 = 0;
    vlSelf->__Vtrigprevexpr___TOP__addr_bound_error__0 = 0;
    vlSelf->__Vtrigprevexpr___TOP__thermal_shutdown_tripped__0 = 0;
    vlSelf->__Vtrigprevexpr___TOP__bram_rdata__0 = 0;
    vlSelf->__VicoDidInit = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggered[__Vi0] = 0;
    }
    vlSelf->__Vtrigprevexpr___TOP__clk__1 = 0;
    vlSelf->__Vtrigprevexpr___TOP__rst__1 = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VnbaTriggered[__Vi0] = 0;
    }
}
