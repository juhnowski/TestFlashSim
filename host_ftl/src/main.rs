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
use types::{alloc_page_aligned, PAGE_SIZE};

fn main() {
    println!("=======================================================");
    println!("🚀 Запуск User-Space Host-Managed FTL на Rust...");
    println!("=======================================================");

    // Подключаемся напрямую к TCP сокету нашего Verilator симулятора
    let stream = TcpStream::connect("127.0.0.1:10809")
        .expect("❌ Ошибка: Не удалось подключиться к симулятору. Убедитесь, что ./run_zns_disk.sh запущен!");

    let mut ftl = HostManagedFtl::new(stream);

    // Выпрямленный синхронный пайплайн тестов без async-рантаймов
    println!("[Тест] 0. Превентивный OOB сброс зоны 0...");
    if let Err(e) = ftl.zone_reset(0) {
        eprintln!("❌ Ошибка превентивного сброса: {:?}", e);
        return;
    }

    let mut write_buffer = alloc_page_aligned(PAGE_SIZE);
    for i in 0..PAGE_SIZE {
        write_buffer[i] = 0x7E;
    }

    let target_logical_page = 0;

    println!(
        "[Тест] 1. Запись по логическому адресу: {}",
        target_logical_page
    );
    match ftl.logical_write(target_logical_page, write_buffer) {
        Ok(_) => {
            println!(
                "[Тест] 2. Чтение из логического адреса: {}",
                target_logical_page
            );
            match ftl.logical_read(target_logical_page) {
                Ok(read_buffer) => {
                    let mut matches = true;
                    for i in 0..PAGE_SIZE {
                        if read_buffer[i] != 0x7E {
                            matches = false;
                            eprintln!("❌ Сбой верификации на байте {}: ожидалось 0x7E, получено 0x{:02X}", i, read_buffer[i]);
                            break;
                        }
                    }
                    if matches {
                        println!(
                            "🎯 ТРИУМФ! Данные успешно прошли цикл трансляции в чистом User-Space!"
                        );
                    }
                }
                Err(e) => eprintln!("❌ Ошибка логического чтения: {:?}", e),
            }
        }
        Err(e) => {
            eprintln!("❌ Ошибка логической записи: {:?}", e);
        }
    }
}
