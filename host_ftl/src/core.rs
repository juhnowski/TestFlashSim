// /home/ilya/TestFlashSim/host_ftl/src/core.rs

pub use crate::ftl_struct::HostManagedFtl;

use crate::mgmt::backend_zone_reset;
use crate::types::NUM_ZONES;

impl HostManagedFtl {
    /// Публичный интерфейс для принудительного сброса зоны — теперь синхронный (убран async)
    pub fn zone_reset(&mut self, zone_id: usize) -> std::io::Result<()> {
        if zone_id >= NUM_ZONES {
            return Err(std::io::Error::new(
                std::io::ErrorKind::InvalidInput,
                "Zone ID out of range",
            ));
        }

        backend_zone_reset(zone_id)?;

        let zone = &mut self.zones[zone_id];
        zone.write_pointer = 0;
        zone.is_full = false;

        self.l2p_table.invalidate_zone(zone_id);

        println!(
            "[FTL Core] Зона {} успешно очищена OOB. Метаданные синхронизированы.",
            zone_id
        );
        Ok(())
    }
}
