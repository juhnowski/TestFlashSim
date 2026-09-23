// /home/ilya/TestFlashSim/host_ftl/src/l2p.rs

use crate::types::{PAGE_SIZE, SECTOR_SIZE, TOTAL_LOGICAL_SECTORS};

#[derive(Clone, Copy, Debug, PartialEq)]
pub struct PhysicalAddress {
    pub zone_id: usize,
    pub page_offset: usize, // 0..64
}

pub struct L2pTable {
    table: Vec<Option<PhysicalAddress>>,
}

impl L2pTable {
    pub fn new() -> Self {
        let total_logical_pages = (TOTAL_LOGICAL_SECTORS * SECTOR_SIZE) / PAGE_SIZE;
        Self {
            table: vec![None; total_logical_pages],
        }
    }

    pub fn get(&self, logical_page_id: usize) -> Option<PhysicalAddress> {
        self.table.get(logical_page_id).copied().flatten()
    }

    pub fn set(&mut self, logical_page_id: usize, paddr: PhysicalAddress) {
        if logical_page_id < self.table.len() {
            self.table[logical_page_id] = Some(paddr);
        }
    }

    pub fn invalidate_zone(&mut self, zone_id: usize) {
        for entry in self.table.iter_mut() {
            if let Some(addr) = entry {
                if addr.zone_id == zone_id {
                    *entry = None;
                }
            }
        }
    }

    pub fn len(&self) -> usize {
        self.table.len()
    }
}
