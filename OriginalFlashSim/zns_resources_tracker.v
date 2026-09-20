// /home/ilya/TestFlashSim/OriginalFlashSim/zns_resources_tracker.v
`timescale 1ns / 1ps

module zns_resources_tracker (
    input  wire       clk,
    input  wire       rst,
    input  wire [7:0] io_cmd,
    input  wire       state_update_pulse,
    input  wire [6:0] zone_wptr,
    input  wire       zone_is_full,

    output reg  [3:0] open_zones_count
);

    always @(posedge clk) begin
        if (rst) begin
            open_zones_count <= 4'd0;
        end else if (state_update_pulse) begin
            if (io_cmd == 8'd1 && zone_wptr == 7'd0 && !zone_is_full) begin
                open_zones_count <= open_zones_count + 4'd1;
            end

            // Если записывается ПОСЛЕДНЯЯ страница (63), зона закрывается, освобождая ресурс
            if (io_cmd == 8'd1 && zone_wptr == 7'd63 && open_zones_count > 0) begin
                open_zones_count <= open_zones_count - 4'd1;
            end else if (io_cmd == 8'd2 && zone_wptr > 7'd0 && !zone_is_full && open_zones_count > 0) begin
                open_zones_count <= open_zones_count - 4'd1;
            end
        end
    end

endmodule
