// /home/ilya/TestFlashSim/OriginalFlashSim/zns_eeprom_controller.v
`timescale 1ns / 1ps

module zns_eeprom_controller (
    input  wire        clk,
    input  wire        rst,

    // Интерфейс прерывания PLP (Power-Loss Protection)
    input  wire        io_plp_trigger,
    input  wire [31:0] io_smart_data,

    // Статусные флаги для хоста
    output reg         out_eeprom_busy,
    output reg         out_eeprom_done,

    // Физический интерфейс SPI EEPROM (SPI Mode 0)
    output reg         spi_sck,
    output reg         spi_mosi,
    output reg         spi_cs_n
);

    // Состояния SPI-автомата
    localparam ST_SPI_IDLE     = 2'd0;
    localparam ST_SPI_START    = 2'd1;
    localparam ST_SPI_TRANSFER = 2'd2;
    localparam ST_SPI_DONE     = 2'd3;

    reg [1:0]  spi_state;
    reg [31:0] shift_reg;
    reg [5:0]  bit_counter;
    reg [2:0]  clk_divider; // Делитель частоты для SCK (100МГц / 8 = 12.5 МГц SPI)

    wire sck_tick = (clk_divider == 3'd3); // Середина полупериода для генерации sck
    wire bit_tick = (clk_divider == 3'd7); // Конец такта бита

    always @(posedge clk or posedge rst) begin
        if (rst) begin
            spi_state       <= ST_SPI_IDLE;
            shift_reg       <= 32'd0;
            bit_counter     <= 6'd0;
            clk_divider     <= 3'd0;
            spi_sck         <= 1'b0;
            spi_mosi        <= 1'b0;
            spi_cs_n        <= 1'b1; // Деактивирован (High)
            out_eeprom_busy <= 1'b0;
            out_eeprom_done <= 1'b0;
        end else begin
            case (spi_state)
                ST_SPI_IDLE: begin
                    spi_sck         <= 1'b0;
                    spi_mosi        <= 1'b0;
                    spi_cs_n        <= 1'b1;
                    out_eeprom_done <= 1'b0;
                    clk_divider     <= 3'd0;

                    if (io_plp_trigger) begin
                        spi_state       <= ST_SPI_START;
                        shift_reg       <= io_smart_data; // Защелкиваем телеметрию в сдвиговый регистр
                        out_eeprom_busy <= 1'b1;
                    end else begin
                        out_eeprom_busy <= 1'b0;
                    end
                end

                ST_SPI_START: begin
                    spi_cs_n    <= 1'b0; // Выбираем чип EEPROM (CS_N -> Low)
                    clk_divider <= clk_divider + 3'd1;
                    spi_mosi    <= shift_reg[31]; // Выставляем старший бит (MSB First)

                    if (bit_tick) begin
                        spi_state   <= ST_SPI_TRANSFER;
                        bit_counter <= 6'd0;
                        clk_divider <= 3'd0;
                    end
                end

                ST_SPI_TRANSFER: begin
                    clk_divider <= clk_divider + 3'd1;

                    if (sck_tick) begin
                        spi_sck <= 1'b1; // Передний фронт SCK (EEPROM читает MOSI)
                    end

                    if (bit_tick) begin
                        spi_sck <= 1'b0; // Спад SCK (Сдвиг данных)
                        if (bit_counter == 6'd31) begin
                            spi_state <= ST_SPI_DONE;
                            spi_mosi  <= 1'b0;
                        end else begin
                            shift_reg   <= {shift_reg[30:0], 1'b0}; // Сдвиг влево
                            bit_counter <= bit_counter + 6'd1;
                            spi_mosi    <= shift_reg[30]; // Выставляем следующий бит
                        end
                        clk_divider <= 3'd0;
                    end
                end

                ST_SPI_DONE: begin
                    spi_cs_n        <= 1'b1; // Гасим CS_N
                    out_eeprom_busy <= 1'b0;
                    out_eeprom_done <= 1'b1; // Сигнализируем об успешном дампе

                    if (!io_plp_trigger) begin
                        spi_state <= ST_SPI_IDLE;
                    end
                end
            endcase
        end
    end

endmodule
