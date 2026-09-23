// /home/ilya/TestFlashSim/host_ftl/src/main.rs

mod ftl;
mod types;

use libc;
use std::fs::OpenOptions;
use std::os::unix::fs::OpenOptionsExt;
use tokio_uring::fs::File;

use ftl::HostManagedFtl;
use types::{alloc_page_aligned, PAGE_SIZE};

fn main() {
    tokio_uring::start(async {
        println!("=======================================================");
        println!("🔄 Верификация сквозного L2P пайплайна (Host-FTL)...");
        println!("=======================================================");

        let std_file = OpenOptions::new()
            .read(true)
            .write(true)
            .custom_flags(libc::O_DIRECT | libc::O_SYNC)
            .open("/dev/nbd0")
            .expect("❌ Ошибка: Не удалось открыть /dev/nbd0. Проверьте run_zns_disk.sh");

        let uring_file = File::from_std(std_file);
        let mut ftl = HostManagedFtl::new(uring_file);

        let mut write_buffer = alloc_page_aligned(PAGE_SIZE);
        for i in 0..PAGE_SIZE {
            write_buffer[i] = 0x7E;
        }

        let target_logical_page = 42;

        println!(
            "[Тест] 1. Запись по логическому адресу: {}",
            target_logical_page
        );
        if let Err(e) = ftl
            .logical_write(target_logical_page, write_buffer.clone())
            .await
        {
            eprintln!("❌ Ошибка логической записи: {:?}", e);
            return;
        }

        println!(
            "[Тест] 2. Чтение из логического адреса: {}",
            target_logical_page
        );
        match ftl.logical_read(target_logical_page).await {
            Ok(read_buffer) => {
                let mut matches = true;
                for i in 0..PAGE_SIZE {
                    if read_buffer[i] != 0x7E {
                        matches = false;
                        eprintln!(
                            "❌ Сбой верификации на байте {}: ожидалось 0x7E, получено 0x{:02X}",
                            i, read_buffer[i]
                        );
                        break;
                    }
                }
                if matches {
                    println!("🎯 ТРИУМФ! Данные успешно прошли цикл трансляции, записи и чтения через Verilator.");
                }
            }
            Err(e) => eprintln!("❌ Ошибка логического чтения: {:?}", e),
        }
    });
}
