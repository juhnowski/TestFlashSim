// /home/ilya/TestFlashSim/OriginalFlashSim/zns_fsm_validator.v
`timescale 1ns / 1ps

module zns_fsm_validator (
    input  wire        clk,
    input  wire        rst,
    input  wire        io_trigger,
    input  wire [7:0]  io_cmd,
    input  wire [31:0] validated_zone_id,
    input  wire [31:0] validated_target_page,
    input  wire        addr_bound_error,
    input  wire        thermal_shutdown_tripped,

    output reg  [31:0] bram_addr,
    input  wire [31:0] bram_rdata,
    output reg  [31:0] bram_wdata,
    output reg         bram_we,

    output reg  [7:0]  out_status,   // 0-READY, 1-FAILURE, 2-BUSY, 3-SUCCESS_DONE
    output reg  [7:0]  out_err_code
);

    // Состояния FSM
    localparam ST_IDLE        = 3'd0;
    localparam ST_BRAM_READ   = 3'd1;
    localparam ST_BRAM_WAIT   = 3'd2;
    localparam ST_VALIDATE    = 3'd3;
    localparam ST_EXECUTE     = 3'd4;
    localparam ST_UPDATE_BRAM = 3'd5;
    localparam ST_ERROR       = 3'd6;

    reg [2:0] current_state, next_state;

    // Безопасная фильтрация X-состояний BRAM
    reg [31:0] safe_bram_rdata;
    integer b;
    always @* begin
        safe_bram_rdata = bram_rdata;
        for (b = 0; b < 32; b = b + 1) begin
            if (bram_rdata[b] === 1'bx || bram_rdata[b] === 1'bz)
                safe_bram_rdata[b] = 1'b0;
        end
    end

    // Распаковка полей
    wire [6:0] zone_wptr     = safe_bram_rdata[6:0];
    wire       zone_is_full   = safe_bram_rdata[7];     // Четкий битовый индекс
    wire [7:0] zone_erase_cnt = safe_bram_rdata[23:16];

    wire [3:0] open_zones_count;
    wire       has_error;
    wire [7:0] internal_error_code;

    wire is_opening_new_zone = (io_cmd == 8'd1) && (zone_wptr == 7'd0) && (!zone_is_full);

    // Аппаратный детектор первого такта вхождения в состояние записи
    reg current_state_d1;
    always @(posedge clk or posedge rst) begin
        if (rst) current_state_d1 <= 1'b0;
        else     current_state_d1 <= (current_state == ST_UPDATE_BRAM);
    end

    wire state_update_pulse;
    assign state_update_pulse = (current_state == ST_UPDATE_BRAM) && !current_state_d1;

    // 1. Подключаем комбинаторный верификатор правил ZNS
    zns_rules_checker u_checker (
        .io_cmd                   (io_cmd),
        .validated_target_page    (validated_target_page),
        .addr_bound_error         (addr_bound_error),
        .thermal_shutdown_tripped (thermal_shutdown_tripped),
        .zone_wptr                (zone_wptr),
        .zone_is_full             (zone_is_full),
        .zone_erase_cnt           (zone_erase_cnt),
        .open_zones_count         (open_zones_count),
        .is_opening_new_zone      (is_opening_new_zone),
        .has_error                (has_error),
        .error_code               (internal_error_code)
    );

    // 2. Подключаем счетчик ресурсов
    zns_resources_tracker u_tracker (
        .clk                (clk),
        .rst                (rst),
        .io_cmd             (io_cmd),
        .state_update_pulse (state_update_pulse),
        .zone_wptr          (zone_wptr),
        .zone_is_full       (zone_is_full),
        .open_zones_count   (open_zones_count)
    );

    always @(posedge clk or posedge rst) begin
        if (rst) current_state <= ST_IDLE;
        else     current_state <= next_state;
    end

    // Handshake архитектура переходов + комбинаторный адрес BRAM
    always @* begin
        if (io_trigger || current_state == ST_BRAM_READ || current_state == ST_BRAM_WAIT || current_state == ST_UPDATE_BRAM) begin
            bram_addr = validated_zone_id;
        end else begin
            bram_addr = 32'd0;
        end

        case (current_state)
            ST_IDLE:        next_state = (io_trigger) ? ST_BRAM_READ : ST_IDLE;
            ST_BRAM_READ:   next_state = ST_BRAM_WAIT;
            ST_BRAM_WAIT:   next_state = ST_VALIDATE;
            ST_VALIDATE:    next_state = (has_error)  ? ST_ERROR : ST_EXECUTE;
            ST_EXECUTE:     next_state = ST_UPDATE_BRAM;

            ST_UPDATE_BRAM: next_state = (io_trigger) ? ST_UPDATE_BRAM : ST_IDLE;
            ST_ERROR:       next_state = (io_trigger) ? ST_ERROR : ST_IDLE;
            default:        next_state = ST_IDLE;
        endcase
    end

    // Синхронный блок выходов
    always @(posedge clk or posedge rst) begin
        if (rst) begin
            out_status   <= 8'd0;
            out_err_code <= 8'd0;
            bram_we      <= 1'b0;
            bram_wdata   <= 32'd0;
        end else begin
            case (current_state)
                ST_IDLE: begin
                    out_status <= 8'd0; // READY
                    bram_we    <= 1'b0;
                end

                ST_BRAM_READ: begin
                    out_status <= 8'd2; // BUSY
                end

                ST_ERROR: begin
                    out_status   <= 8'd1; // FAILURE
                    out_err_code <= internal_error_code;
                end

                ST_UPDATE_BRAM: begin
                    out_status   <= 8'd3; // SUCCESS_DONE
                    out_err_code <= 8'd00;

                    // КРИТИЧЕСКОЕ ИСПРАВЛЕНИЕ: Выставляем строб bram_we строго по одиночному импульсу
                    if (state_update_pulse) begin
                        bram_we <= 1'b1;
                        if (io_cmd == 8'd1) begin
                            if (zone_wptr == 7'd63)
                                bram_wdata <= {zone_erase_cnt, 8'd0, 1'b1, 7'd64};
                            else
                                bram_wdata <= {zone_erase_cnt, 8'd0, 1'b0, (zone_wptr + 7'd1)};
                        end else if (io_cmd == 8'd2) begin
                            bram_wdata <= {8'd0, (zone_erase_cnt + 8'd1), 16'd0};
                        end
                    end else begin
                        bram_we <= 1'b0; // На следующем такте мгновенно гасим строб записи
                    end
                end
            endcase
        end
    end

endmodule
