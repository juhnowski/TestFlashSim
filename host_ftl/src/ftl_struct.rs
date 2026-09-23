// /home/ilya/TestFlashSim/host_ftl/src/ftl_struct.rs

use crate::l2p::L2pTable;
use crate::types::{ZoneDescriptor, NUM_ZONES};
use std::net::TcpStream;

pub struct HostManagedFtl {
    pub net_stream: TcpStream, // ИСПРАВЛЕНО: Теперь работаем напрямую через TCP-сокет бэкенда!
    pub zones: Vec<ZoneDescriptor>,
    pub l2p_table: L2pTable,
    pub current_write_zone: usize,
}

impl HostManagedFtl {
    pub fn new(stream: TcpStream) -> Self {
        let mut zones = Vec::with_capacity(NUM_ZONES);
        for id in 0..NUM_ZONES {
            zones.push(ZoneDescriptor {
                id,
                write_pointer: 0,
                is_full: false,
            });
        }

        Self {
            net_stream: stream,
            zones,
            l2p_table: L2pTable::new(),
            current_write_zone: 0,
        }
    }
}
