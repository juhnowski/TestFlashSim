// /home/ilya/TestFlashSim/OriginalFlashSim/zns_address_resolver.v
`timescale 1ns / 1ps

module zns_address_resolver (
    input  wire        clk,
    input  wire        rst,
    input  wire [63:0] io_lba,
    input  wire [5:0]  cfg_zone_shift,
    input  wire [31:0] cfg_zone_size_mask,
    input  wire [31:0] cfg_total_zones,

    output reg  [31:0] out_zone_id,
    output reg  [31:0] out_target_page,
    output reg         out_error_out_of_bounds
);

    wire [63:0] w_zone_id_calc;
    wire [63:0] w_target_page_calc;

    // Безопасный сдвиг: если в управляющих сигналах мусор, принудительно выдаем 0
    assign w_zone_id_calc     = (^cfg_zone_shift === 1'bX) ? 64'd0 : (io_lba >> cfg_zone_shift);
    assign w_target_page_calc = io_lba & cfg_zone_size_mask;

    always @(posedge clk) begin
        if (rst) begin
            out_zone_id             <= 32'd0;
            out_target_page         <= 32'd0;
            out_error_out_of_bounds <= 1'b0;
        end else begin
            // Проверяем на корректность: если вычисленное значение ванильно и корректно
            if (^w_zone_id_calc === 1'bX || ^cfg_total_zones === 1'bX) begin
                out_error_out_of_bounds <= 1'b0;
                out_zone_id             <= 32'd0;
                out_target_page         <= 32'd0;
            end else begin
                out_zone_id     <= w_zone_id_calc[31:0];
                out_target_page <= w_target_page_calc[31:0];

                // Компаратор жестких границ
                if (w_zone_id_calc >= cfg_total_zones) begin
                    out_error_out_of_bounds <= 1'b1;
                end else begin
                    out_error_out_of_bounds <= 1'b0;
                end
            end
        end
    end

endmodule
