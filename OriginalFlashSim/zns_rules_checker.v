// /home/ilya/TestFlashSim/OriginalFlashSim/zns_rules_checker.v
`timescale 1ns / 1ps

module zns_rules_checker (
    input  wire [7:0]  io_cmd,
    input  wire [31:0] validated_target_page,
    input  wire        addr_bound_error,
    input  wire        thermal_shutdown_tripped,

    input  wire [6:0]  zone_wptr,
    input  wire        zone_is_full,
    input  wire [7:0]  zone_erase_cnt,
    input  wire [3:0]  open_zones_count,
    input  wire        is_opening_new_zone,

    output reg         has_error,
    output reg  [7:0]  error_code
);

    wire [3:0] safe_open_zones;
    assign safe_open_zones = open_zones_count;

    wire [31:0] zone_wptr_32;
    assign zone_wptr_32 = {25'd0, zone_wptr};

    always @* begin
        if (thermal_shutdown_tripped) begin
            has_error  = 1'b1;
            error_code = 8'd08; // Перегрев
        end else if (io_cmd == 8'd99) begin
            has_error  = 1'b1;
            error_code = 8'd06; // Невалидная команда
        end else if (io_cmd == 8'd1 && zone_is_full) begin
            has_error  = 1'b1;
            error_code = 8'd07; // Запись в полную зону
        end else if (io_cmd == 8'd1 && (validated_target_page != zone_wptr_32)) begin
            has_error  = 1'b1;
            error_code = 8'd01; // Внеочередная запись
        end else if (io_cmd == 8'd1 && is_opening_new_zone && (safe_open_zones >= 4)) begin
            has_error  = 1'b1;
            error_code = 8'd02; // Превышен лимит открытых зон
        end else if (io_cmd == 8'd0 && (validated_target_page >= zone_wptr_32)) begin
            has_error  = 1'b1;
            error_code = 8'd03; // Чтение пустой области
        end else if (io_cmd == 8'd2 && (zone_erase_cnt >= 8'd10)) begin // ИСПРАВЛЕНО: Порог поднят до 10 стираний!
            has_error  = 1'b1;
            error_code = 8'd05; // Превышен ресурс стираний кремния
        end else if (addr_bound_error) begin
            has_error  = 1'b1;
            error_code = 8'd07; // Выход за общую геометрию
        end else begin
            has_error  = 1'b0;
            error_code = 8'd00;
        end
    end

endmodule
