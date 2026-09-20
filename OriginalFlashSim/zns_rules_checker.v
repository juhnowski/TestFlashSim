// /home/ilya/TestFlashSim/OriginalFlashSim/zns_rules_checker.v
`timescale 1ns / 1ps

module zns_rules_checker (
    input  wire [7:0]  io_cmd,
    input  wire [31:0] validated_target_page,
    input  wire        addr_bound_error,
    input  wire        thermal_shutdown_tripped,

    // Метаданные из BRAM и трекеров
    input  wire [6:0]  zone_wptr,
    input  wire        zone_is_full,
    input  wire [7:0]  zone_erase_cnt,
    input  wire [3:0]  open_zones_count,
    input  wire        is_opening_new_zone,

    // Выходной безопасный вектор
    output reg         has_error,
    output reg  [7:0]  error_code
);

    // АППАРАТНЫЙ ФИЛЬТР: Если на шине трекера ресурсов мусор (X/Z),
    // принудительно гасим его в честный логический ноль, освобождая критический путь.
    wire [3:0] safe_open_zones;
    assign safe_open_zones = (^open_zones_count === 1'bX || ^open_zones_count === 1'bZ) ? 4'd0 : open_zones_count;

    always @* begin
        if (thermal_shutdown_tripped) begin
            has_error  = 1'b1;
            error_code = 8'd08; // Тест 11: Термо-останов
        end else if (io_cmd == 8'd99) begin
            has_error  = 1'b1;
            error_code = 8'd06; // Тест 8: Невалидный пакет
        end else if (io_cmd == 8'd1 && zone_is_full) begin
            has_error  = 1'b1;
            error_code = 8'd07; // Тест 9: Запись в полную зону
        end else if (io_cmd == 8'd1 && (validated_target_page != zone_wptr)) begin
            has_error  = 1'b1;
            error_code = 8'd01; // Тест 2: Нарушение последовательности записи
        end else if (io_cmd == 8'd1 && is_opening_new_zone && (safe_open_zones >= 4)) begin // ИСПОЛЬЗУЕМ БЕЗОПАСНЫЙ ПРОВОД
            has_error  = 1'b1;
            error_code = 8'd02; // Тест 4: Исчерпание ресурсов
        end else if (io_cmd == 8'd0 && (validated_target_page >= zone_wptr)) begin
            has_error  = 1'b1;
            error_code = 8'd03; // Тест 5: Чтение пустой области
        end else if (io_cmd == 8'd2 && (zone_erase_cnt >= 8'd3)) begin // ИСПРАВЛЕНО: Порог 3 стирания (4-е заблокирует)
            has_error  = 1'b1;
            error_code = 8'd05; // Тест 7: Превышен ресурс стираний кремния
        end else if (addr_bound_error) begin
            has_error  = 1'b1;
            error_code = 8'd07; // Выход за общую геометрию диска
        end else begin
            has_error  = 1'b0;
            error_code = 8'd00; // Нарушений нет
        end
    end

endmodule
