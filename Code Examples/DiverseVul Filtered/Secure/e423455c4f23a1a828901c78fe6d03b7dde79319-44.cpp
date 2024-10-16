int rom_check_and_register_reset(void)
{
    hwaddr addr = 0;
    MemoryRegionSection section;
    Rom *rom;
    AddressSpace *as = NULL;

    QTAILQ_FOREACH(rom, &roms, next) {
        if (rom->fw_file) {
            continue;
        }
        if (!rom->mr) {
            if ((addr > rom->addr) && (as == rom->as)) {
                fprintf(stderr, "rom: requested regions overlap "
                        "(rom %s. free=0x" TARGET_FMT_plx
                        ", addr=0x" TARGET_FMT_plx ")\n",
                        rom->name, addr, rom->addr);
                return -1;
            }
            addr  = rom->addr;
            addr += rom->romsize;
            as = rom->as;
        }
        section = memory_region_find(rom->mr ? rom->mr : get_system_memory(),
                                     rom->addr, 1);
        rom->isrom = int128_nz(section.size) && memory_region_is_rom(section.mr);
        memory_region_unref(section.mr);
    }
    qemu_register_reset(rom_reset, NULL);
    roms_loaded = 1;
    return 0;
}