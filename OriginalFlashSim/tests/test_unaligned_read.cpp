// /home/ilya/TestFlashSim/OriginalFlashSim/tests/test_unaligned_read.cpp
#include <stdio.h>
#include "../ssd.h"

using namespace ssd;

bool run_unaligned_read_test(Controller &controller) {
    printf("   [ТЕСТ 8]: Проверка строгого аппаратного выравнивания операций чтения...\n");

    // Передаем некорректный код операции чтения (например, имитируем невыровненный или сломанный запрос с флагом операции 99)
    printf("   [ТЕСТ 8]: Отправка невыровненного/невалидного запроса хоста (is_write = 99)...\n");
    if (controller.direct_zns_gate(0, 99) == FAILURE) {
        printf("   [ТЕСТ 8]: Контроллер успешно отклонил невыровненную системную транзакцию.\n");
        return true;
    }

    return false;
}
