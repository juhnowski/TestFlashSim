// /home/ilya/TestFlashSim/host_ftl/src/types.rs

pub const NUM_ZONES: usize = 1024;
pub const ZONE_SIZE_PAGES: usize = 64;
pub const PAGE_SIZE: usize = 4096;
pub const ZONE_SIZE_BYTES: u64 = (ZONE_SIZE_PAGES * PAGE_SIZE) as u64;

pub const SECTOR_SIZE: usize = 512;
pub const TOTAL_LOGICAL_SECTORS: usize = (NUM_ZONES * ZONE_SIZE_PAGES * PAGE_SIZE) / SECTOR_SIZE;

#[derive(Clone, Copy, Debug, PartialEq)]
pub struct PhysicalAddress {
    pub zone_id: usize,
    pub page_offset: usize, // 0..64
}

#[derive(Debug)]
pub struct ZoneDescriptor {
    pub id: usize,
    pub write_pointer: u64,
    pub is_full: bool,
}

pub fn alloc_page_aligned(size: usize) -> Vec<u8> {
    let layout = std::alloc::Layout::from_size_align(size, PAGE_SIZE).unwrap();
    unsafe {
        let ptr = std::alloc::alloc(layout);
        Vec::from_raw_parts(ptr, size, size)
    }
}
