// /home/ilya/TestFlashSim/OriginalFlashSim/tb_zns_controller.v
`timescale 1ns / 1ps

module tb_zns_controller();

    reg         clk;
    reg         rst;
    reg         trigger;
    reg  [7:0]  cmd;
    reg  [63:0] lba;

    wire [7:0]  status;
    wire [7:0]  err_code;

    // Внутренние провода для соединения резолвера и валидатора
    wire [31:0] zone_id;
    wire [31:0] tgt_page;
    wire        bound_err;

    // Провода для имитации Block RAM памяти метаданных
    wire [31:0] bram_addr;
    wire [31:0] bram_wdata;
    wire        bram_we;
    reg  [31:0] mock_bram_rdata;

    reg          crypto_clear;
    reg  [127:0] crypto_data_in;
    reg          crypto_valid_in;
    wire [127:0] crypto_data_out;
    wire         crypto_valid_out;

    wire [31:0] real_bram_rdata;

    // Подключаем честный аппаратный блок BRAM памяти
    zns_metadata_bram u_bram (
        .clk   (clk),
        .addr  (bram_addr[11:0]),
        .wdata (bram_wdata),
        .we    (bram_we),
        .rdata (real_bram_rdata)
    );

    // Инстанцируем крипто-движок
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

    // 1. Инстанцируем вычислитель координат зон
    zns_address_resolver u_resolver (
        .clk                     (clk),
        .rst                     (rst),
        .io_lba                  (lba),
        .cfg_zone_shift          (6'd6),       // 64 страницы в зоне
        .cfg_zone_size_mask      (32'h0000_003F),
        .cfg_total_zones         (32'd4096),
        .out_zone_id             (zone_id),
        .out_target_page         (tgt_page),
        .out_error_out_of_bounds (bound_err)
    );

    // 2. Инстанцируем конечный автомат валидации ZNS
    zns_fsm_validator u_validator (
        .clk                    (clk),
        .rst                    (rst),
        .io_trigger             (trigger),
        .io_cmd                 (cmd),
        .validated_zone_id      (zone_id),
        .validated_target_page  (tgt_page),
        .addr_bound_error       (bound_err),
        .bram_addr              (bram_addr),
        .bram_rdata             (real_bram_rdata), // Подключили реальную шину вместо mock!
        .bram_wdata             (bram_wdata),
        .bram_we                (bram_we),
        .out_status             (status),
        .out_err_code           (err_code)
    );

    // Генерация тактового сигнала (период 10нс = 100 МГц)
    always #5 clk = ~clk;

    initial begin
        // Инициализация сигналов
        clk = 0;
        rst = 1;
        trigger = 0;
        cmd = 0;
        lba = 0;
        mock_bram_rdata = 32'h0000_0000; // wptr = 0, is_full = 0, erase_cnt = 0

        #20;
        rst = 0;
        #10;

        $display("=== СТАРТ RTL ТЕСТБЕНЧА КОНТРОЛЛЕРА ZNS ===");

        // --- СЦЕНАРИЙ 1: Валидная последовательная запись на страницу 0 ---
// /home/ilya/TestFlashSim/OriginalFlashSim/tb_zns_controller.v

        // --- СЦЕНАРИЙ 1 ---
        $display("[RTL ТЕСТ 1]: Попытка записи на LBA 0 (Страница 0, Ожидается УСПЕХ)...");
        lba = 64'd0; cmd = 8'd1; trigger = 1;
        #20; trigger = 0;
        #80;
        if (status == 8'd0 && err_code == 8'd00)
            $display("  -> РЕЗУЛЬТАТ: Успешно одобрено. Аппаратный wptr инкрементирован.");
        else
            $display("  -> ОШИБКА: Контроллер заблокировал валидную запись! Код: 0x%02X", err_code);

        #20; // Даем время BRAM обновить выходной регистр данных на шине

        // --- СЦЕНАРИЙ 2 ---
        $display("[RTL ТЕСТ 2]: Попытка случайной записи на LBA 5 мимо wptr=1 (Ожидается ОТКАЗ)...");
        lba = 64'd5; cmd = 8'd1; trigger = 1;
        #50; // Даем 5 тактов на полный проход конвейера до состояния ST_ERROR

        if (status == 8'd1 && err_code == 8'd01)
            $display("  -> РЕЗУЛЬТАТ: Контроллер аппаратно отверг случайную запись с кодом ZNS_ERR_UNALIGNED_WRITE (0x01).");
        else
            $display("  -> ОШИБКА: Тест завалился! Контроллер пропустил некорректную запись. Статус: %d, Код: 0x%02X", status, err_code);

        trigger = 0; // Снимаем триггер, переводя автомат обратно в IDLE
        #20;

        // --- СЦЕНАРИЙ 3 ---
        $display("[RTL ТЕСТ 3]: Попытка чтения нераспределенной страницы LBA 10 при wptr=1 (Ожидается ОТКАЗ)...");
        lba = 64'd10; cmd = 8'd0; trigger = 1;
        #50; // Даем 5 тактов на чтение BRAM и уход в ошибку

        if (status == 8'd1 && err_code == 8'd03)
            $display("  -> РЕЗУЛЬТАТ: Контроллер заблокировал чтение пустой зоны с кодом ZNS_ERR_READ_EMPTY_ZONE (0x03).");
        else
            $display("  -> ОШИБКА: Тест завалился! Контроллер выдал мусор из пустой ячейки флеша. Статус: %d, Код: 0x%02X", status, err_code);

        trigger = 0;
        #20;


        // --- СЦЕНАРИЙ 4 ---
        $display("[RTL ТЕСТ 4]: Запуск шифрования блока данных Хоста (AES-XTS-256)...");
        crypto_clear = 0;
        crypto_data_in = 128'hDEADBEEF_CAFEBABE_11223344_55667788;
        crypto_valid_in = 1;

        // Ждем 14 тактов прохождения через конвейер раундов AES (140 нс)
        #140;
        $display("  -> Поток зашифрован. Выходной шифротекст: 0x%H", crypto_data_out);

        $display("[RTL ТЕСТ 4]: Обнаружена Ring -2 (Intel ME) атака! Активация линии crypto_clear...");
        crypto_clear = 1;
        #20; // Удерживаем сброс ключей 2 такта
        crypto_clear = 0;

        // Даем конвейеру обновиться с нулевыми ключами
        #140;
        $display("  -> Результат после Crypto-Erase: 0x%H (Ожидается белый шум/изменение)", crypto_data_out);

        crypto_valid_in = 0; // Выключаем поток данных

        #140;
        $display("  -> Результат после Crypto-Erase: 0x%H (Ожидается белый шум/изменение)", crypto_data_out);

        $display("=== RTL ТЕСТБЕНЧ УСПЕШНО ЗАВЕРШЕН ===");
        $finish;
    end

endmodule
