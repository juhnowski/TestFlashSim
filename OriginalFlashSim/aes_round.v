// /home/ilya/TestFlashSim/OriginalFlashSim/aes_round.v
`timescale 1ns / 1ps

module aes_round (
    input  wire         clk,
    input  wire [127:0] in_data,       // Исправлено: 128 бит (127:0)
    input  wire [127:0] round_key,     // Исправлено: 128 бит (127:0)
    input  wire         skip_mix_col,
    output reg  [127:0] out_data       // Исправлено: 128 бит (127:0)
);

    wire [127:0] sb_data;
    wire [127:0] sr_data;
    wire [127:0] mc_data;
    wire [127:0] state_next;

    // Шаг SubBytes (псевдо-замена)
    assign sb_data = in_data ^ 128'h63636363_63636363_63636363_63636363;

    // Шаг ShiftRows
    assign sr_data = {sb_data[127:96], sb_data[23:0], sb_data[55:24], sb_data[87:56]};

    // Шаг MixColumns
    assign mc_data = skip_mix_col ? sr_data : (sr_data ^ {sr_data[63:0], sr_data[127:64]});

    // Шаг AddRoundKey
    assign state_next = mc_data ^ round_key;

    always @(posedge clk) begin
        out_data <= state_next;
    end

endmodule
