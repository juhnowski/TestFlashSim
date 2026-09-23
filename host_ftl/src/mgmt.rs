// /home/ilya/TestFlashSim/host_ftl/src/mgmt.rs

use crate::types::NUM_ZONES;
use std::io::{Read, Write};
use std::os::unix::io::AsRawFd;
use std::os::unix::net::UnixStream;

// Системный макрос Linux для жесткой очистки буферов и триггеров ошибок блочного устройства
const BLKFLSBUF: std::os::raw::c_ulong = 0x1261;

/// Прямой аппаратный сброс зоны Verilator через выделенный сокет управления
pub fn backend_zone_reset(zone_id: usize) -> std::io::Result<()> {
    if zone_id >= NUM_ZONES {
        return Err(std::io::Error::new(
            std::io::ErrorKind::InvalidInput,
            "Zone ID out of range",
        ));
    }

    println!("[Mgmt-Driver] Подключение к каналу управления /tmp/zns_mgmt.sock...");
    let mut stream = UnixStream::connect("/tmp/zns_mgmt.sock")?;
    stream.set_read_timeout(Some(std::time::Duration::from_secs(2)))?;

    let zone_data = (zone_id as u64).to_ne_bytes();
    stream.write_all(&zone_data)?;
    stream.flush()?;

    println!(
        "[Mgmt-Driver] ID зоны ({}) отправлен. Ожидаем ACK от Verilator...",
        zone_id
    );
    let mut ack = [0u8; 1];
    stream.read_exact(&mut ack)?;

    println!(
        "[Mgmt-Driver] Аппаратный сброс подтвержден кремнием. ACK: {}",
        ack[0]
    );
    Ok(())
}

/// Жесткий сброс внутреннего кэша ошибок (ENOSPC/EIO) подсистемы VFS ядра Linux
// ИСПРАВЛЕНО: Исправлено имя аргумента с std::_file на нормальный std_file
pub fn clear_kernel_block_device_errors(std_file: &std::fs::File) {
    let fd = std_file.as_raw_fd();
    println!("[Mgmt-Driver] Отправка системного вызова ioctl(BLKFLSBUF) для разблокировки ядра...");
    unsafe {
        libc::ioctl(fd, BLKFLSBUF, 0);
    }
}
