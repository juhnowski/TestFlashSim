// /home/ilya/TestFlashSim/OriginalFlashSim/zns_fsm_validator.v
`timescale 1ns / 1ps

/* verilator lint_off SYNCASYNCNET */
/* verilator lint_off CASEINCOMPLETE */
/* verilator lint_off UNUSEDSIGNAL */

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

    output reg  [7:0]  out_status,
    output reg  [7:0]  out_err_code
);

    wire [31:0] safe_bram_rdata;
    assign safe_bram_rdata = bram_rdata;

    wire [6:0] zone_wptr     = safe_bram_rdata[6:0];
    wire       zone_is_full   = safe_bram_rdata[7];
    wire [7:0] zone_erase_cnt = safe_bram_rdata[23:16];

    localparam ST_IDLE        = 3'd0;
    localparam ST_BRAM_READ   = 3'd1;
    localparam ST_BRAM_WAIT   = 3'd2;
    localparam ST_VALIDATE    = 3'd3;
    localparam ST_EXECUTE     = 3'd4;
    localparam ST_UPDATE_BRAM = 3'd5;
    localparam ST_ERROR       = 3'd6;

    reg [2:0] current_state, next_state;
    wire [3:0] open_zones_count;
    wire       has_error;
    wire [7:0] internal_error_code;

    wire is_opening_new_zone = (io_cmd == 8'd1) && (zone_wptr == 7'd0) && (!zone_is_full);

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

    zns_resources_tracker u_tracker (
        .clk                (clk),
        .rst                (rst),
        .io_cmd             (io_cmd),
        .state_update_pulse (current_state == ST_EXECUTE),
        .zone_wptr          (zone_wptr),
        .zone_is_full       (zone_is_full),
        .open_zones_count   (open_zones_count)
    );

    always @(posedge clk or posedge rst) begin
        if (rst) current_state <= ST_IDLE;
        else     current_state <= next_state;
    end

    // ИСПРАВЛЕНО: Заменена фигурная скобка на законный end в блоке переходов!
    always @* begin
        if (current_state != ST_IDLE || io_trigger) begin
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

    always @(posedge clk or posedge rst) begin
        if (rst) begin
            out_status   <= 8'd0;
            out_err_code <= 8'd0;
            bram_we      <= 1'b0;
            bram_wdata   <= 32'd0;
        end else begin
            case (current_state)
                ST_IDLE: begin
                    out_status <= 8'd0;
                    bram_we    <= 1'b0;
                end

                ST_BRAM_READ: out_status <= 8'd2;

                ST_ERROR: begin
                    out_status   <= 8'd1;
                    out_err_code <= internal_error_code;
                end

                ST_EXECUTE: begin
                    bram_we <= 1'b1;
                    if (io_cmd == 8'd1) begin
                        if (zone_wptr == 7'd63)
                            bram_wdata <= {8'd0, zone_erase_cnt, 8'd0, 1'b1, 7'd64};
                        else
                            bram_wdata <= {8'd0, zone_erase_cnt, 8'd0, 1'b0, {25'd0, (zone_wptr + 7'd1)}[6:0]};
                    end else if (io_cmd == 8'd2) begin
                        bram_wdata <= {8'd0, (zone_erase_cnt + 8'd1), 16'd0};
                    end
                end

                ST_UPDATE_BRAM: begin
                    out_status   <= 8'd3;
                    out_err_code <= 8'd00;
                    bram_we      <= 1'b0;
                end
            endcase
        end
    end

endmodule

/* verilator lint_on SYNCASYNCNET */
/* verilator lint_on CASEINCOMPLETE */
/* verilator lint_on UNUSEDSIGNAL */
