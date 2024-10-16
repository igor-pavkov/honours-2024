static void rom_reset(void *unused)
{
    Rom *rom;

    /*
     * We don't need to fill in the RAM with ROM data because we'll fill
     * the data in during the next incoming migration in all cases.  Note
     * that some of those RAMs can actually be modified by the guest on ARM
     * so this is probably the only right thing to do here.
     */
    if (runstate_check(RUN_STATE_INMIGRATE))
        return;

    QTAILQ_FOREACH(rom, &roms, next) {
        if (rom->fw_file) {
            continue;
        }
        if (rom->data == NULL) {
            continue;
        }
        if (rom->mr) {
            void *host = memory_region_get_ram_ptr(rom->mr);
            memcpy(host, rom->data, rom->datasize);
        } else {
            address_space_write_rom(rom->as, rom->addr, MEMTXATTRS_UNSPECIFIED,
                                    rom->data, rom->datasize);
        }
        if (rom->isrom) {
            /* rom needs to be written only once */
            rom_free_data(rom);
        }
        /*
         * The rom loader is really on the same level as firmware in the guest
         * shadowing a ROM into RAM. Such a shadowing mechanism needs to ensure
         * that the instruction cache for that new region is clear, so that the
         * CPU definitely fetches its instructions from the just written data.
         */
        cpu_flush_icache_range(rom->addr, rom->datasize);

        trace_loader_write_rom(rom->name, rom->addr, rom->datasize, rom->isrom);
    }
}