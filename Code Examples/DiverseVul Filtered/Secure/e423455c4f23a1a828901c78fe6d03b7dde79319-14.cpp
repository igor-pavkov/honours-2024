static void rom_insert(Rom *rom)
{
    Rom *item;

    if (roms_loaded) {
        hw_error ("ROM images must be loaded at startup\n");
    }

    /* The user didn't specify an address space, this is the default */
    if (!rom->as) {
        rom->as = &address_space_memory;
    }

    rom->committed = false;

    /* List is ordered by load address in the same address space */
    QTAILQ_FOREACH(item, &roms, next) {
        if (rom_order_compare(rom, item)) {
            continue;
        }
        QTAILQ_INSERT_BEFORE(item, rom, next);
        return;
    }
    QTAILQ_INSERT_TAIL(&roms, rom, next);
}