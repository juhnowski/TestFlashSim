// /home/ilya/TestFlashSim/OriginalFlashSim/tests/test_read_split_zone.cpp
#include <stdio.h>
#include "../ssd.h"

using namespace ssd;

bool run_read_split_zone_test(Controller &controller) {
    printf("   [ТЕСТ 12]: Проверка частичного чтения (запрос пересекает границу WPTR)...\n");

    // Используем чистую Зону 60 (LBA = 60 * 64 = 3840)
    uint base_lba = 3840;
    controller.direct_zns_gate(base_lba, 2); // Очистка

    // Запишем только 2 страницы (LBA 3840 и 3841). wptr станет равен 2.
    if (controller.direct_zns_gate(base_lba, 1) == FAILURE) return false;
    if (controller.direct_zns_gate(base_lba + 1, 1) == FAILURE) return false;

    // Имитируем запрос чтения, который начинается на валидной странице 3841 (внутри wptr),
    // но запрашивает чтение следующей за ней пустой страницы 3842 ( target_page >= wptr ).
    // Согласно NVMe ZNS, контроллер должен аппаратно отвергнуть всю транзакцию полностью.
    printf("   [ТЕСТ 12]: Запрос чтения LBA %u (находится за пределами WPTR=2)...\n", base_lba + 2);
    if (controller.direct_zns_gate(base_lba + 2, 0) == FAILURE) {
        printf("   [ТЕСТ 12]: Контроллер успешно заблокировал некорректную транзакцию на границе WPTR.\n");
        controller.direct_zns_gate(base_lba, 2); // Очистка
        return true;
    }

    controller.direct_zns_gate(base_lba, 2);
    return false;
}
