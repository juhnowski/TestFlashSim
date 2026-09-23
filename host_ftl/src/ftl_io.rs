// /home/ilya/TestFlashSim/host_ftl/src/ftl_io.rs

use crate::core::HostManagedFtl;
use crate::types::{PAGE_SIZE, ZONE_SIZE_BYTES};
use std::io::{Read, Write};

// Магическое число запроса NBD протокола
const ZNS_NBD_REQUEST_MAGIC: u32 = 0x25609513;

impl HostManagedFtl {
    /// Физическая отправка Append-пакета напрямую в C++ сервер через TCP-сокет
    pub fn append_to_zone(&mut self, zone_id: usize, data: Vec<u8>) -> std::io::Result<u64> {
        let zone = &mut self.zones[zone_id];
        let global_offset = (zone_id as u64 * ZONE_SIZE_BYTES) + zone.write_pointer;

        // Формируем упакованный NBD запрос вручную
        let mut request_bytes = Vec::with_capacity(28);
        request_bytes.extend_from_slice(&ZNS_NBD_REQUEST_MAGIC.to_be_bytes()); // magic
        request_bytes.extend_from_slice(&1u32.to_be_bytes()); // type (1 = WRITE)
        request_bytes.extend_from_slice(&0u64.to_be_bytes()); // handle
        request_bytes.extend_from_slice(&global_offset.to_be_bytes()); // from
        request_bytes.extend_from_slice(&(data.len() as u32).to_be_bytes()); // len

        // Шлем заголовок и payload данных
        self.net_stream.write_all(&request_bytes)?;
        self.net_stream.write_all(&data)?;
        self.net_stream.flush()?;

        // Читаем NBD ответ от C++ сервера (16 байт структуры reply)
        let mut reply_buf = [0u8; 16];
        self.net_stream.read_exact(&mut reply_buf)?;

        // Извлекаем код ошибки (байты 4..8 структуры reply)
        let error_code =
            u32::from_be_bytes([reply_buf[4], reply_buf[5], reply_buf[6], reply_buf[7]]);
        if error_code != 0 {
            return Err(std::io::Error::new(
                std::io::ErrorKind::StorageFull,
                format!("RTL заблокировал запись, код: {}", error_code),
            ));
        }

        zone.write_pointer += data.len() as u64;
        if zone.write_pointer >= ZONE_SIZE_BYTES {
            zone.is_full = true;
        }
        Ok(zone.write_pointer)
    }

    /// Высокоуровневое логическое чтение — теперь чистая синхронная функция
    pub fn logical_read(&mut self, logical_page_id: usize) -> std::io::Result<Vec<u8>> {
        if logical_page_id >= self.l2p_table.len() {
            return Err(std::io::Error::new(
                std::io::ErrorKind::InvalidInput,
                "Logical page ID out of bounds",
            ));
        }

        let paddr = match self.l2p_table.get(logical_page_id) {
            Some(addr) => addr,
            None => return Ok(vec![0u8; PAGE_SIZE]),
        };

        let global_offset =
            (paddr.zone_id as u64 * ZONE_SIZE_BYTES) + (paddr.page_offset * PAGE_SIZE) as u64;

        // Формируем NBD запрос на чтение (type = 0)
        let mut request_bytes = Vec::with_capacity(28);
        request_bytes.extend_from_slice(&ZNS_NBD_REQUEST_MAGIC.to_be_bytes()); // magic
        request_bytes.extend_from_slice(&0u32.to_be_bytes()); // type (0 = READ)
        request_bytes.extend_from_slice(&0u64.to_be_bytes()); // handle
        request_bytes.extend_from_slice(&global_offset.to_be_bytes()); // from
        request_bytes.extend_from_slice(&(PAGE_SIZE as u32).to_be_bytes()); // len

        self.net_stream.write_all(&request_bytes)?;
        self.net_stream.flush()?;

        // Читаем NBD ответ
        let mut reply_buf = [0u8; 16];
        self.net_stream.read_exact(&mut reply_buf)?;

        let mut read_data = vec![0u8; PAGE_SIZE];
        self.net_stream.read_exact(&mut read_data)?;

        println!(
            "[L2P] Чтение: LBA {} считан с адреса [Зона: {}, Страница: {}]",
            logical_page_id, paddr.zone_id, paddr.page_offset
        );
        Ok(read_data)
    }
}
