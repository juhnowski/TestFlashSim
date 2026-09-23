// /home/ilya/TestFlashSim/host_ftl/src/ftl.rs

use crate::types::{
    PhysicalAddress, ZoneDescriptor, NUM_ZONES, PAGE_SIZE, SECTOR_SIZE, TOTAL_LOGICAL_SECTORS,
    ZONE_SIZE_BYTES,
};
use tokio_uring::fs::File;

pub struct HostManagedFtl {
    dev_file: File,
    pub zones: Vec<ZoneDescriptor>,
    pub l2p_table: Vec<Option<PhysicalAddress>>,
    current_write_zone: usize,
}

impl HostManagedFtl {
    pub fn new(file: File) -> Self {
        let mut zones = Vec::with_capacity(NUM_ZONES);
        for id in 0..NUM_ZONES {
            zones.push(ZoneDescriptor {
                id,
                write_pointer: 0,
                is_full: false,
            });
        }

        // ДОБАВЛЕНО: Теперь SECTOR_SIZE виден в этой области
        let total_logical_pages = (TOTAL_LOGICAL_SECTORS * SECTOR_SIZE) / PAGE_SIZE;

        Self {
            dev_file: file,
            zones,
            l2p_table: vec![None; total_logical_pages],
            current_write_zone: 0,
        }
    }

    /// Логическая асинхронная запись (Host-FTL Write)
    pub async fn logical_write(
        &mut self,
        logical_page_id: usize,
        data: Vec<u8>,
    ) -> std::io::Result<()> {
        if logical_page_id >= self.l2p_table.len() {
            return Err(std::io::Error::new(
                std::io::ErrorKind::InvalidInput,
                "Logical page ID out of bounds",
            ));
        }

        let mut zone_id = self.current_write_zone;
        while self.zones[zone_id].is_full {
            zone_id += 1;
            if zone_id >= NUM_ZONES {
                return Err(std::io::Error::new(
                    std::io::ErrorKind::StorageFull,
                    "All ZNS zones are full",
                ));
            }
        }
        self.current_write_zone = zone_id;

        let page_offset = (self.zones[zone_id].write_pointer / PAGE_SIZE as u64) as usize;

        // Попытка физической записи с перехватом аппаратной блокировки RTL
        match self.append_to_zone(zone_id, data.clone()).await {
            Ok(_) => {}
            Err(ref e)
                if e.kind() == std::io::ErrorKind::StorageFull || e.raw_os_error() == Some(28) =>
            {
                println!("⚠️  [FTL] RTL заблокировал запись (ENOSPC). Инициализируем Out-of-Band сброс зоны {}...", zone_id);

                // Автоматически очищаем контроллер через наш Out-of-Band канал
                self.zone_reset(zone_id).await?;

                println!(
                    "🚀 [FTL] Повторная отправка страницы в очищенную зону {}...",
                    zone_id
                );
                self.append_to_zone(zone_id, data).await?;
            }
            Err(e) => return Err(e),
        }

        // Обновляем L2P таблицу только при успешной физической записи
        self.l2p_table[logical_page_id] = Some(PhysicalAddress {
            zone_id,
            page_offset,
        });

        println!(
            "[L2P Mapping] Маппинг сохранен: LBA {} -> Физический [Зона: {}, Страница: {}]",
            logical_page_id, zone_id, page_offset
        );

        Ok(())
    }

    /// Логическое асинхронное чтение (Host-FTL Read)
    pub async fn logical_read(&self, logical_page_id: usize) -> std::io::Result<Vec<u8>> {
        if logical_page_id >= self.l2p_table.len() {
            return Err(std::io::Error::new(
                std::io::ErrorKind::InvalidInput,
                "Logical page ID out of bounds",
            ));
        }

        let paddr = match self.l2p_table[logical_page_id] {
            Some(addr) => addr,
            None => return Ok(vec![0u8; PAGE_SIZE]),
        };

        let global_offset =
            (paddr.zone_id as u64 * ZONE_SIZE_BYTES) + (paddr.page_offset * PAGE_SIZE) as u64;
        let buf = crate::types::alloc_page_aligned(PAGE_SIZE);

        // ИСПРАВЛЕНИЕ E0599: Убран вызов .submit() для операции read_at, так как она сразу возвращает Future
        let (res, returned_buf) = self.dev_file.read_at(buf, global_offset).await;
        res?;

        println!(
            "[L2P Mapping] Чтение: LBA {} с адреса [Зона: {}, Страница: {}]",
            logical_page_id, paddr.zone_id, paddr.page_offset
        );

        Ok(returned_buf)
    }

    /// Внутренний метод физического аппенда
    async fn append_to_zone(&mut self, zone_id: usize, data: Vec<u8>) -> std::io::Result<u64> {
        let zone = &mut self.zones[zone_id];
        let global_offset = (zone_id as u64 * ZONE_SIZE_BYTES) + zone.write_pointer;

        let (res, _buf) = self.dev_file.write_at(data, global_offset).submit().await;
        let bytes_written = res?;

        zone.write_pointer += bytes_written as u64;
        if zone.write_pointer >= ZONE_SIZE_BYTES {
            zone.is_full = true;
        }
        Ok(zone.write_pointer)
    }

    /// Аппаратный сброс зоны (Out-of-Band)
    pub async fn zone_reset(&mut self, zone_id: usize) -> std::io::Result<()> {
        if zone_id >= NUM_ZONES {
            return Err(std::io::Error::new(
                std::io::ErrorKind::InvalidInput,
                "Zone ID out of range",
            ));
        }

        use std::io::{Read, Write};
        use std::os::unix::net::UnixStream;

        println!("[Host-FTL] Подключение к каналу управления /tmp/zns_mgmt.sock...");
        let mut stream = UnixStream::connect("/tmp/zns_mgmt.sock")?;

        // Задаем таймаут на чтение, чтобы если C++ сервер завис, тест не блокировал консоль навсегда
        stream.set_read_timeout(Some(std::time::Duration::from_secs(2)))?;

        let zone_data = (zone_id as u64).to_ne_bytes();

        // Пишем данные и принудительно заставляем ОС выполнить сисвызов отправки
        stream.write_all(&zone_data)?;
        stream.flush()?;

        println!(
            "[Host-FTL] Данные ID зоны ({}) отправлены. Ожидаем байт подтверждения (ACK)...",
            zone_id
        );

        let mut ack = [0u8; 1];
        stream.read_exact(&mut ack)?;

        let zone = &mut self.zones[zone_id];
        zone.write_pointer = 0;
        zone.is_full = false;

        for entry in self.l2p_table.iter_mut() {
            if let Some(addr) = entry {
                if addr.zone_id == zone_id {
                    *entry = None;
                }
            }
        }

        println!(
            "[Host-FTL] Метаданные зоны {} и L2P ссылки очищены. ACK: {}",
            zone_id, ack[0]
        );
        Ok(())
    }
}
