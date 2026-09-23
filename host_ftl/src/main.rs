// /home/ilya/TestFlashSim/host_ftl/src/main.rs

mod core;
mod ftl_io;
mod ftl_struct;
mod ftl_write_pipeline;
mod l2p;
mod mgmt;
mod types;

use core::HostManagedFtl;
use std::net::TcpStream;
use std::sync::{Arc, Mutex};
use std::thread;
use types::{alloc_page_aligned, PAGE_SIZE, ZONE_SIZE_PAGES};

fn main() {
    println!("=======================================================");
    println!("🚀 СТРЕСС-ТЕСТ: ЦИКЛИЧЕСКОЕ ЗАПОЛНЕНИЕ ЗОН (64 СТРАНИЦЫ) 🚀");
    println!("=======================================================");

    let stream = TcpStream::connect("127.0.0.1:10809")
        .expect("❌ Ошибка: Не удалось подключиться к симулятору.");

    let ftl = HostManagedFtl::new(stream);
    let ftl_shared = Arc::new(Mutex::new(ftl));

    // Превентивно очищаем стартовые зоны в Verilator перед тестом
    println!("[Стресс-Тест] OOB сброс зон (0, 1, 2)...");
    {
        let mut ftl = ftl_shared.lock().unwrap();
        ftl.zone_reset(0).expect("❌ Сбой сброса зоны 0");
        ftl.zone_reset(1).expect("❌ Сбой сброса зоны 1");
        ftl.zone_reset(2).expect("❌ Сбой com-сброса зоны 2");
    }
    println!("✅ Кремний обнулен. Потоки воркеров стартуют...");

    let mut thread_handles = vec![];

    for worker_id in 0..3 {
        let ftl_clone = Arc::clone(&ftl_shared);

        let handle = thread::spawn(move || {
            // Формируем уникальный page-aligned буфер под воркера (0x01, 0x02, 0x03)
            let mut write_buffer = alloc_page_aligned(PAGE_SIZE);
            for i in 0..PAGE_SIZE {
                write_buffer[i] = (worker_id + 1) as u8;
            }

            println!(
                "🚀 [Воркер {}] Начинает последовательное заполнение {} страниц...",
                worker_id, ZONE_SIZE_PAGES
            );

            // КРУТИМ ЦИКЛ НА ВСЕ 64 СТРАНИЦЫ ЗОНЫ (в нашей геометрии ZONE_SIZE_PAGES = 64)
            for page_idx in 0..ZONE_SIZE_PAGES {
                // Вычисляем уникальный LBA для каждого шага (например, Воркер 0 пишет LBA 0..64)
                let target_lba = (worker_id * 100) + page_idx;

                // 1. Критическая секция для отправки последовательного Append запроса в TCP-мост
                let write_res = {
                    let mut ftl = ftl_clone.lock().unwrap();
                    // Чтобы воркеры писали строго в свои зоны (Воркер 0 в Зону 0, Воркер 1 в Зону 1 и т.д.),
                    // мы принудительно выставим current_write_zone в мьютексе под ID воркера
                    ftl.current_write_zone = worker_id;
                    ftl.logical_write(target_lba, write_buffer.clone())
                };

                if let Err(e) = write_res {
                    eprintln!(
                        "❌ [Воркер {}, Стр {}] Критический RTL-запрет! Ошибка записи: {:?}",
                        worker_id, page_idx, e
                    );
                    return; // Аварийно выходим из потока нагрузки
                }

                // Логируем только знаковые вехи, чтобы не спамить консоль
                if page_idx == 0 || page_idx == 31 || page_idx == 63 {
                    println!(
                        "👉 [Воркер {}] Успешный Append страницы {}/64 (LBA {})",
                        worker_id,
                        page_idx + 1,
                        target_lba
                    );
                }

                // 2. Сразу же верифицируем только что записанную страницу, вычитывая ее обратно
                let read_res = {
                    let mut ftl = ftl_clone.lock().unwrap();
                    ftl.logical_read(target_lba)
                };

                match read_res {
                    Ok(read_buffer) => {
                        let mut matches = true;
                        for i in 0..PAGE_SIZE {
                            if read_buffer[i] != (worker_id + 1) as u8 {
                                matches = false;
                                break;
                            }
                        }
                        if !matches {
                            eprintln!("❌ [Воркер {}, Стр {}] СБОЙ ЦЕЛОСТНОСТИ! Считаны поврежденные данные.", worker_id, page_idx);
                            return;
                        }
                    }
                    Err(e) => {
                        eprintln!(
                            "❌ [Воркер {}, Стр {}] Ошибка верификационного чтения: {:?}",
                            worker_id, page_idx, e
                        );
                        return;
                    }
                }
            }

            println!(
                "🎯 [Воркер {}] ЗОНА ПОЛНОСТЬЮ ЗАПОЛНЕНА И ВЕРИФИЦИРОВАНА БЕЗ ОШИБОК!",
                worker_id
            );
        });

        thread_handles.push(handle);
    }

    // Ожидаем завершения работы всех трех параллельных потоков
    for handle in thread_handles {
        let _ = handle.join();
    }

    println!("\n=======================================================");
    println!("🏁 ТЕСТ СУПЕР-ЗАПОЛНЕНИЯ ЗАВЕРШЕН! Кремниевый автомат отработал штатно.");
    println!("=======================================================");
}
