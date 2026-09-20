`timescale 1ns / 1ps

module tb_zns_controller();

    reg         clk;
    reg         rst;
    reg         trigger;
    reg  [7:0]  cmd;
    reg  [63:0] lba;
    reg  [7:0]  raw_temp;

    wire [7:0]  status;
    wire [7:0]  err_code;

    wire [31:0] zone_id;
    wire [31:0] tgt_page;
    wire        bound_err;

    wire [31:0] bram_addr;
    wire [31:0] bram_wdata;
    wire        bram_we;
    wire [31:0] real_bram_rdata;

    reg          test_failed;

    reg          crypto_clear;
    reg  [127:0] crypto_data_in;
    reg          crypto_valid_in;
    wire [127:0] crypto_data_out;
    wire         crypto_valid_out;

    // Сигналы для Теста SPI EEPROM (Тест 10 в C++)
    reg         plp_trigger;
    reg  [31:0] smart_data;
    wire        eeprom_busy;
    wire        eeprom_done;
    wire        spi_sck;
    wire        spi_mosi;
    wire        spi_cs_n;

    // 1. Инстанцируем резолвер адресов
    zns_address_resolver u_resolver (
        .clk                     (clk),
        .rst                     (rst),
        .io_lba                  (lba),
        .cfg_zone_shift          (6'd6),
        .cfg_zone_size_mask      (32'h0000_003F),
        .cfg_total_zones         (32'd4096),
        .out_zone_id             (zone_id),
        .out_target_page         (tgt_page),
        .out_error_out_of_bounds (bound_err)
    );

    // 2. Инстанцируем Block RAM
    zns_metadata_bram u_bram (
        .clk   (clk),
        .addr  (bram_addr[11:0]),
        .wdata (bram_wdata),
        .we    (bram_we),
        .rdata (real_bram_rdata)
    );

    // 3. Инстанцируем тепловой датчик
    wire thermal_trip;
    wire [7:0] thermal_err;
    zns_thermal_manager u_thermal (
        .clk                         (clk),
        .rst                         (rst),
        .raw_temperature             (raw_temp),
        .out_thermal_shutdown_tripped (thermal_trip),
        .out_thermal_err_code         (thermal_err)
    );

    // 4. Инстанцируем диспетчер автомата ZNS
    zns_fsm_validator u_validator (
        .clk                    (clk),
        .rst                    (rst),
        .io_trigger             (trigger),
        .io_cmd                 (cmd),
        .validated_zone_id      (zone_id),
        .validated_target_page  (tgt_page),
        .addr_bound_error       (bound_err),
        .thermal_shutdown_tripped (thermal_trip),
        .bram_addr              (bram_addr),
        .bram_rdata             (real_bram_rdata),
        .bram_wdata             (bram_wdata),
        .bram_we                (bram_we),
        .out_status             (status),
        .out_err_code           (err_code)
    );

    // 5. Инстанцируем крипто-движок
    zns_crypto_engine u_crypto (
        .clk                (clk),
        .rst                (rst),
        .crypto_clear       (crypto_clear),
        .io_data_in         (crypto_data_in),
        .io_lba             (lba),
        .io_valid_in        (crypto_valid_in),
        .out_data_encrypted (crypto_data_out),
        .out_valid_out      (crypto_valid_out)
    );

    // 6. Инстанцируем контроллер энергонезависимой памяти EEPROM
    zns_eeprom_controller u_eeprom (
        .clk             (clk),
        .rst             (rst),
        .io_plp_trigger  (plp_trigger),
        .io_smart_data   (smart_data),
        .out_eeprom_busy (eeprom_busy),
        .out_eeprom_done (eeprom_done),
        .spi_sck         (spi_sck),
        .spi_mosi        (spi_mosi),
        .spi_cs_n        (spi_cs_n)
    );

    // Генерация тактового сигнала 100 МГц
    always #5 clk = ~clk;

    // ОСНОВНОЙ ПРОЦЕДУРНЫЙ БЛОК ВЕРИФИКАЦИИ
    initial begin
        clk = 0;
        rst = 1;
        trigger = 0;
        cmd = 0;
        lba = 0;
        raw_temp = 8'd35;
        test_failed = 0;
        crypto_clear = 0;
        crypto_data_in = 0;
        crypto_valid_in = 0;
        plp_trigger = 0;
        smart_data = 32'd0;

        #40;
        rst = 0;
        #20;

        $display("=== СТАРТ RTL ТЕСТБЕНЧА КОНТРОЛЛЕРА ZNS ===");

        // --- СЦЕНАРИЙ 1 ---
        $display("[RTL ТЕСТ 1]: Попытка записи на LBA 0 (Страница 0, Ожидается УСПЕХ)...");
        lba = 64'h0000_0000_0000_0000; cmd = 8'd1; trigger = 1;
        #100;
        if (status == 8'd3 && err_code == 8'd00)
            $display("  -> РЕЗУЛЬТАТ: УСПЕШНО! Успешно одобрено. Аппаратный wptr инкрементирован.");
        else begin
            $display("  -> ОШИБКА: Контроллер заблокировал валидную запись! Статус: %d, Код: 0x%02X", status, err_code);
            test_failed = 1;
        end
        trigger = 0; #60;

        // --- СЦЕНАРИЙ 2 ---
        $display("[RTL ТЕСТ 2]: Попытка случайной записи на LBA 5 мимо wptr=1 (Ожидается ОТКАЗ)...");
        lba = 64'h0000_0000_0000_0005; cmd = 8'd1; trigger = 1;
        #100;
        if (status == 8'd1 && err_code == 8'd01)
            $display("  -> РЕЗУЛЬТАТ: Контроллер аппаратно отверг случайную запись с кодом ZNS_ERR_UNALIGNED_WRITE (0x01).");
        else begin
            $display("  -> ОШИБКА: Тест завалился! Статус: %d, Код: 0x%02X", status, err_code);
            test_failed = 1;
        end
        trigger = 0; #60;

        // --- СЦЕНАРИЙ 3 ---
        $display("[RTL ТЕСТ 3]: Попытка чтения нераспределенной страницы LBA 10 при wptr=1 (Ожидается ОТКАЗ)...");
        lba = 64'h0000_0000_0000_000A; cmd = 8'd0; trigger = 1;
        #100;
        if (status == 8'd1 && err_code == 8'd03)
            $display("  -> РЕЗУЛЬТАТ: Контроллер заблокировал чтение пустой зоны с кодом ZNS_ERR_READ_EMPTY_ZONE (0x03).");
        else begin
            $display("  -> ОШИБКА: Тест завалился! Статус: %d, Код: 0x%02X", status, err_code);
            test_failed = 1;
        end
        trigger = 0; #60;

        // --- СЦЕНАРИЙ 4 ---
        $display("[RTL ТЕСТ 4]: Запуск шифрования блока данных Хоста (AES-XTS-256)...");
        crypto_clear = 0; crypto_data_in = 128'hDEADBEEF_CAFEBABE_11223344_55667788; crypto_valid_in = 1;
        #140;
        $display("  -> Поток зашифрован. Выходной шифротекст: 0x%H", crypto_data_out);
        crypto_clear = 1; #20; crypto_clear = 0; #140;
        $display("  -> Результат после Crypto-Erase: 0x%H (Ожидается белый шум/изменение)", crypto_data_out);
        crypto_valid_in = 0; #20;

        // --- СЦЕНАРИЙ 5 ---
        $display("[RTL ТЕСТ 5]: Симуляция термического разгона чипа до 90°C (Критический порог: 85°C)...");
        raw_temp = 8'd90; #20;
        lba = 64'h0000_0000_0000_0000; cmd = 8'd1; trigger = 1;
        #100;
        if (status == 8'd1 && err_code == 8'd08)
            $display("  -> РЕЗУЛЬТАТ: Контроллер заблокировал I/O конвейер с кодом ZNS_ERR_THERMAL_SHUTDOWN (0x08). Кремний спасен!");
        else begin
            $display("  -> ОШИБКА: Защита проигнорировала перегрев! Статус: %d, Код: 0x%02X", status, err_code);
            test_failed = 1;
        end
        trigger = 0; raw_temp = 8'd35; #60;

        // --- СЦЕНАРИЙ 6 ---
        $display("[RTL ТЕСТ 6]: Попытка исчерпания ресурсов (открытие 5 зон подряд при лимите 4)...");
        lba = 64'h0000_0000_0000_0040; cmd = 8'd1; trigger = 1; #100; trigger = 0; #60;
        lba = 64'h0000_0000_0000_0080; cmd = 8'd1; trigger = 1; #100; trigger = 0; #60;
        lba = 64'h0000_0000_0000_00C0; cmd = 8'd1; trigger = 1; #100; trigger = 0; #60;

        lba = 64'h0000_0000_0000_0100; cmd = 8'd1; trigger = 1;
        #100;
        if (status == 8'd1 && err_code == 8'd02)
            $display("  -> РЕЗУЛЬТАТ: Контроллер успешно заблокировал 5-ю зону с кодом ZNS_ERR_RESOURCE_EXCEEDED (0x02).");
        else begin
            $display("  -> ОШИБКА: Контроллер пропустил превышение лимита! Статус: %d, Код: 0x%02X", status, err_code);
            test_failed = 1;
        end
        trigger = 0; #60;

        // --- СЦЕНАРИЙ 7 ---
        $display("[RTL ТЕСТ 7]: Стресс-тест деградации кремния (стирание Зоны 10 ровно 5 раз)...");
        lba = 64'h0000_0000_0000_0280;

        cmd = 8'd2; trigger = 1; #100; trigger = 0; cmd = 8'd0; #60;
        cmd = 8'd2; trigger = 1; #100; trigger = 0; cmd = 8'd0; #60;
        cmd = 8'd2; trigger = 1; #100; trigger = 0; cmd = 8'd0; #60;
        cmd = 8'd2; trigger = 1; #100; trigger = 0; cmd = 8'd0; #60;

        cmd = 8'd2; trigger = 1; #100;
        if (status == 8'd1 && err_code == 8'd05)
            $display("  -> РЕЗУЛЬТАТ: Сработал Wear Guard. Износ зафиксирован с кодом ZNS_WARN_WEAR_LIMIT (0x05).");
        else begin
            $display("  -> ОШИБКА: Контроллер проигнорировал износ ячеек! Статус: %d, Код: 0x%02X", status, err_code);
            test_failed = 1;
        end
        trigger = 0; cmd = 8'd0; #60;

        // --- СЦЕНАРИЙ 8 ---
        $display("[RTL ТЕСТ 8]: Тестирование защиты от перезаписи полной зоны (Зона 20)...");
        lba = 64'h0000_0000_0000_0500;
        u_bram.ram[20] = 32'h0000_0080;
        #20;
        cmd = 8'd1; trigger = 1;
        #100;
        if (status == 8'd1 && err_code == 8'd07)
            $display("  -> РЕЗУЛЬТАТ: Контроллер аппаратно отверг запись в полную область с кодом ZNS_ERR_WRITE_TO_FULL (0x07).");
        else begin
            $display("  -> ОШИБКА: Контроллер допустил разрушение данных полной зоны! Статус: %d, Код: 0x%02X", status, err_code);
            test_failed = 1;
        end
        trigger = 0; #60;

        // --- СЦЕНАРИЙ 9: Экстренный SPI-сброс SMART телеметрии при аварии PLP ---
        $display("[RTL ТЕСТ 9]: Обнаружено падение напряжения питания! Активация линии PLP...");
        plp_trigger = 1;
        smart_data = 32'h01_2A_0004; // 1 ошибка, 42 градуса, износ 4
        #20;

        if (eeprom_busy == 1'b1 && spi_cs_n == 1'b0) begin
            $display("  -> Аппаратный затвор SPI сработал. Линия CS_N прижата к земле.");
        end else begin
            $display("  -> ОШИБКА: SPI контроллер проигнорировал прерывание PLP!");
test_failed = 1;
end
#3500; // Ждем передачи 32 бит данных по SPI
if (eeprom_done == 1'b1)
begin
$display("  -> РЕЗУЛЬТАТ: SMART-телеметрия успешно сериализована в EEPROM. Данные спасены!");
end else begin
$display("  -> ОШИБКА: Контроллер завис во время передачи дампа!");
test_failed = 1;
end
plp_trigger = 0;
#100;

if (test_failed) begin
$display("\n❌ [RTL CRITICAL ERROR]: Обнаружен провал тестов внутри верификационной сюиты!");
end
$display("=== RTL ТЕСТБЕНЧ УСПЕШНО ЗАВЕРШЕН ===");
$finish;
end
// ИЗОЛИРОВАННЫЙ ОТЛАДОЧНЫЙ МОНИТОР ВЕРХНЕГО УРОВНЯ
initial begin
forever begin
#5;
if (cmd == 8'd2) begin
$display("   [LOG]: %t ps | State: %d | WE: %b | Addr: %d | RDATA: 32'h%H | Trig: %b | CMD: %d", $time, u_validator.current_state, bram_we, bram_addr, real_bram_rdata, trigger, cmd);
end
end
end
endmodule
