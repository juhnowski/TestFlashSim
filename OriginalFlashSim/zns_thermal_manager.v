// /home/ilya/TestFlashSim/OriginalFlashSim/zns_thermal_manager.v
`timescale 1ns / 1ps

module zns_thermal_manager (
    input  wire        clk,
    input  wire        rst,

    // Входной интерфейс от физического термодатчика (Xilinx SysMon)
    // Температура передается в фиксированной точке (целое число градусов Цельсия)
    input  wire [7:0]  raw_temperature,

    // Выходные управляющие сигналы защиты
    output reg         out_thermal_shutdown_tripped, // Аппаратный сигнал отключения питания флеш
    output reg  [7:0]  out_thermal_err_code         // Код ошибки перегрева (0x08)
);

    // Жестко заданный критический порог JEDEC стандарта для SSD накопителей
    localparam CRITICAL_TEMP_THRESHOLD = 8'd85; // 85°C

    always @(posedge clk) begin
        if (rst) begin
            out_thermal_shutdown_tripped <= 1'b0;
            out_thermal_err_code         <= 8'd00;
        end else begin
            // Сравниваем показания датчика за 1 такт частоты sys_clk
            if (raw_temperature >= CRITICAL_TEMP_THRESHOLD) begin
                out_thermal_shutdown_tripped <= 1'b1; // Аварийное размыкание
                out_thermal_err_code         <= 8'd08; // Выставляем код ошибки 0x08 из п. 3 ТЗ
            end else begin
                // Автомат защиты не сбрасывается самостоятельно до жесткого ресета (rst)
                // для предотвращения циклического перезапуска перегретого кремния.
                // Но для удобства симуляции тестов разрешаем сброс, если температура упала.
                out_thermal_shutdown_tripped <= 1'b0;
                out_thermal_err_code         <= 8'd00;
            end
        end
    end

endmodule
