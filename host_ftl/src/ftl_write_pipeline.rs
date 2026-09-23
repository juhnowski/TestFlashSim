// /home/ilya/TestFlashSim/host_ftl/src/ftl_write_pipeline.rs

use crate::core::HostManagedFtl;
use crate::l2p::PhysicalAddress;
use crate::types::{NUM_ZONES, PAGE_SIZE};

impl HostManagedFtl {
    /// Высокоуровневая логическая запись в чистом user-space пространстве
    pub fn logical_write(
        &mut self,
        logical_page_id: usize,
        data: Vec<u8>,
    ) -> std::io::Result<Vec<u8>> {
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

        let current_buf = data;

        // Убрали асинхронность: теперь это прямой и надежный вызов сокета
        match self.append_to_zone(zone_id, current_buf.clone()) {
            Ok(_) => {}
            Err(_) => {
                println!("⚠️  [FTL Core] RTL заблокировал запись. Активация OOB сброса зоны через Mgmt сокет...");

                // Убрали .await
                self.zone_reset(zone_id)?;

                println!("🚀 [FTL Core] Повторная чистая отправка страницы в сокет симулятора...");
                self.append_to_zone(zone_id, current_buf.clone())?;
            }
        }

        self.l2p_table.set(
            logical_page_id,
            PhysicalAddress {
                zone_id,
                page_offset,
            },
        );
        println!(
            "[L2P] УСПЕХ: LBA {} -> Транслирован на [Зона: {}, Страница: {}]",
            logical_page_id, zone_id, page_offset
        );
        Ok(current_buf)
    }
}
