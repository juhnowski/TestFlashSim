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

    output reg  [31:0] bram_addr,
    input  wire [31:0] bram_rdata,
    output reg  [31:0] bram_wdata,
    output reg         bram_we,

    output reg  [7:0]  out_status,
    output reg  [7:0]  out_err_code
);

    localparam ST_RESET       = 3'd0;
    localparam ST_IDLE        = 3'd1;
    localparam ST_BRAM_READ   = 3'd2;
    localparam ST_BRAM_WAIT   = 3'd3;
    localparam ST_VALIDATE    = 3'd4;
    localparam ST_EXECUTE     = 3'd5;
    localparam ST_UPDATE_BRAM = 3'd6;
    localparam ST_ERROR       = 3'd7;

    reg [2:0] current_state;
    reg [2:0] next_state;

    wire [6:0]  zone_wptr;
    wire        zone_is_full;
    wire [7:0]  zone_erase_cnt;

    assign zone_wptr      = bram_rdata[6:0];
    assign zone_is_full   = bram_rdata[7];
    assign zone_erase_cnt = bram_rdata[23:16];

    always @(posedge clk or posedge rst) begin
        if (rst) current_state <= ST_RESET;
        else     current_state <= next_state;
    end

    always @(*) begin
        case (current_state)
            ST_RESET:      next_state = ST_IDLE;
            ST_IDLE:       next_state = (io_trigger) ? ST_BRAM_READ : ST_IDLE;
            ST_BRAM_READ:  next_state = ST_BRAM_WAIT;
            ST_BRAM_WAIT:  next_state = ST_VALIDATE;
            ST_VALIDATE: begin
                if (addr_bound_error) next_state = ST_ERROR;
                else if (io_cmd == 8'd99) next_state = ST_ERROR;
                else if (io_cmd == 8'd1 && zone_is_full) next_state = ST_ERROR;
                else if (io_cmd == 8'd1 && (validated_target_page != zone_wptr)) next_state = ST_ERROR;
                else if (io_cmd == 8'd0 && (validated_target_page >= zone_wptr)) next_state = ST_ERROR;
                else next_state = ST_EXECUTE;
            end
            ST_EXECUTE:     next_state = ST_UPDATE_BRAM;
            ST_UPDATE_BRAM: next_state = ST_IDLE;
            ST_ERROR: next_state = (io_trigger) ? ST_ERROR : ST_IDLE;
            default:        next_state = ST_IDLE;
        endcase
    end

    always @(posedge clk) begin
        if (rst) begin
            out_status   <= 8'd0;
            out_err_code <= 8'd0;
            bram_we      <= 1'b0;
        end else begin
            case (current_state)
                ST_IDLE: begin
                    out_status <= 8'd0; // READY
                    bram_we    <= 1'b0;
                end
                ST_BRAM_READ: begin
                    out_status <= 8'd2; // BUSY
                    bram_addr  <= validated_zone_id;
                end
                ST_ERROR: begin
                    out_status <= 8'd1; // ИСПРАВЛЕНИЕ: Жестко выставляем статус FAILURE (1) для хоста!
                    if (addr_bound_error)    out_err_code <= 8'd07;
                    else if (io_cmd == 8'd99) out_err_code <= 8'd06;
                    else if (zone_is_full)   out_err_code <= 8'd07;
                    else if (io_cmd == 8'd1)  out_err_code <= 8'd01; // ZNS_ERR_UNALIGNED_WRITE
                    else if (io_cmd == 8'd0)  out_err_code <= 8'd03; // ZNS_ERR_READ_EMPTY_ZONE
                end
                ST_UPDATE_BRAM: begin
                    bram_addr <= validated_zone_id;
                    bram_we   <= 1'b1; // Взводим строб записи синхронно
                    if (io_cmd == 8'd1) begin
                        bram_wdata <= {zone_erase_cnt, 8'd0, (zone_wptr == 7'd63), (zone_wptr + 7'd1)};
                    end else if (io_cmd == 8'd2) begin
                        bram_wdata <= {(zone_erase_cnt + 8'd1), 24'd0};
                    end
                    out_err_code <= 8'd00; // SUCCESS
                end
            endcase
        end
    end
endmodule
