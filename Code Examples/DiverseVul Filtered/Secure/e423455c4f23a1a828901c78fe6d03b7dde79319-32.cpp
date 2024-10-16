static Rom *find_rom(hwaddr addr, size_t size)
{
    Rom *rom;

    QTAILQ_FOREACH(rom, &roms, next) {
        if (rom->fw_file) {
            continue;
        }
        if (rom->mr) {
            continue;
        }
        if (rom->addr > addr) {
            continue;
        }
        if (rom->addr + rom->romsize < addr + size) {
            continue;
        }
        return rom;
    }
    return NULL;
}