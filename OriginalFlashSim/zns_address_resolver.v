// /home/ilya/TestFlashSim/OriginalFlashSim/zns_address_resolver.v
`timescale 1ns / 1ps

module zns_address_resolver (
    input  wire        clk,
    input  wire        rst,

    // Интерфейс данных хоста
    input  wire [63:0] io_lba,

    // Регистры динамической конфигурации прошивки (п. 2.1 ТЗ)
    input  wire [5:0]  cfg_zone_shift,     // log2(BLOCK_SIZE), например 6 для размера 64 страницы
    input  wire [31:0] cfg_zone_size_mask, // BLOCK_SIZE - 1, например 32'h0000_003F для размера 64 страницы
    input  wire [31:0] cfg_total_zones,    // Максимальное число зон из конфига

    // Выходные вычисленные координаты
    output reg  [31:0] out_zone_id,
    output reg  [31:0] out_target_page,
    output reg         out_error_out_of_bounds // Флаг ошибки превышения геометрии (Код 0x07)
);

    // Внутренние комбинаторные провода
    wire [63:0] w_zone_id_calc;
    wire [63:0] w_target_page_calc;

    // Математика зон без тяжелых операций деления и остатка:
    // Динамический сдвиг вправо заменяет деление на BLOCK_SIZE
    assign w_zone_id_calc     = io_lba >> cfg_zone_shift;

    // Побитовое И с маской заменяет взятие остатка от деления
    assign w_target_page_calc = io_lba & cfg_zone_size_mask;

    always @(posedge clk) begin
        if (rst) begin
            out_zone_id             <= 32'd0;
            out_target_page         <= 32'd0;
            out_error_out_of_bounds <= 1'b0;
        end else begin
            out_zone_id     <= w_zone_id_calc[31:0];
            out_target_page <= w_target_page_calc[31:0];

            // Защитный компаратор: проверяем, что вычисленный zone_id не выходит за границы прошивки
            if (w_zone_id_calc >= cfg_total_zones) begin
                out_error_out_of_bounds <= 1'b1; // Взводим аппаратный флаг отказа
            end else begin
                out_error_out_of_bounds <= 1'b0;
            end
        end
    end

endmodule
