// /home/ilya/TestFlashSim/OriginalFlashSim/zns_crypto_engine.v
`timescale 1ns / 1ps

module zns_crypto_engine (
    input  wire         clk,
    input  wire         rst,
    input  wire         crypto_clear,
    input  wire [127:0] io_data_in,
    input  wire [63:0]  io_lba,
    input  wire         io_valid_in,
    output wire [127:0] out_data_encrypted,
    output reg          out_valid_out
);

    reg [255:0] key_data;
    reg [255:0] key_tweak;

    // Сброс и экстренное зануление ключей (Crypto-Erase)
    always @(posedge clk) begin
        if (rst || crypto_clear) begin
            key_data  <= 256'd0;
            key_tweak <= 256'd0;
        end else begin
            if (key_data == 256'd0) begin
                key_data  <= 256'hA5A5A5A5_B4B4B4B4_C3C3C3C3_D2D2D2D2_E1E1E1E1_F0F0F0F0_0F0F0F0F_1E1E1E1E;
                key_tweak <= 256'h11223344_55667788_99A0A0A0_B0B0B0B0_C0C0C0C0_D0D0D0D0_E0E0E0E0_F0F0F0F0;
            end
        end
    end

    wire [127:0] tweak_mask;
    assign tweak_mask = {io_lba, io_lba} ^ key_tweak[127:0];

    wire [127:0] xts_input;
    assign xts_input = io_data_in ^ tweak_mask;

    // Массив из 15 элементов для 14 ступеней конвейера
    wire [127:0] r_data [0:14];

    // ИСПРАВЛЕНИЕ 1: Явно указываем начальный индекс [0] для XOR с начальным ключом
    assign r_data[0] = xts_input ^ key_data[127:0];

    genvar i;
    generate
        for (i = 0; i < 13; i = i + 1) begin : aes_pipeline_stages
            aes_round round_inst (
                .clk          (clk),
                .in_data      (r_data[i]),
                .round_key    (key_data[127:0] ^ (i + 1)),
                .skip_mix_col (1'b0),
                .out_data     (r_data[i+1])
            );
        end
    endgenerate

    // ИСПРАВЛЕНИЕ 2: 14-й финальный раунд берет данные из r_data[13] и кладет в r_data[14]
    aes_round final_round_inst (
        .clk          (clk),
        .in_data      (r_data[13]),
        .round_key    (key_data[255:128]),
        .skip_mix_col (1'b1),
        .out_data     (r_data[14])
    );

    // ИСПРАВЛЕНИЕ 3: Выходной блок берет данные строго из финальной точки конвейера r_data[14]
    assign out_data_encrypted = r_data[14] ^ tweak_mask;

    // Сдвиговый регистр валидности
    reg [13:0] valid_delay_reg;
    always @(posedge clk) begin
        if (rst) begin
            key_data  <= 256'hA5A5A5A5_B4B4B4B4_C3C3C3C3_D2D2D2D2_E1E1E1E1_F0F0F0F0_0F0F0F0F_1E1E1E1E;
            key_tweak <= 256'h11223344_55667788_99A0A0A0_B0B0B0B0_C0C0C0C0_D0D0D0D0_E0E0E0E0_F0F0F0F0;
        end else begin
            if (crypto_clear) begin
                // При атаке Ring -2 ключи уничтожаются безвозвратно в ноль
                key_data  <= 256'h00000000_00000000_00000000_00000000_00000000_00000000_00000000_00000000;
                key_tweak <= 256'h00000000_00000000_00000000_00000000_00000000_00000000_00000000_00000000;
            end
        end
    end

endmodule
