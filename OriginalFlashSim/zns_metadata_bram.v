// /home/ilya/TestFlashSim/OriginalFlashSim/zns_metadata_bram.v
`timescale 1ns / 1ps

module zns_metadata_bram (
    input  wire        clk,
    input  wire [11:0] addr,     // 4096 ячеек = 12-битный адрес
    input  wire [31:0] wdata,
    input  wire        we,
    output reg  [31:0] rdata
);

    // Выделяем массив памяти внутри FPGA (4096 ячеек по 32 бита)
    reg [31:0] ram [0:4095];

    // ИСПРАВЛЕНИЕ 1: Совместимый с iverilog метод очистки массива через readmemh или простой цикл
    integer i;
    initial begin
        for (i = 0; i < 4096; i = i + 1) begin
            ram[i] = 32'h0000_0000;
        end
    end

    // ИСПРАВЛЕНИЕ 2: Чтение данных привязываем строго к регистровому выходу внутри такта
    always @(posedge clk) begin
        if (we) begin
            ram[addr] <= wdata;
        end
        rdata <= ram[addr];
    end

endmodule
