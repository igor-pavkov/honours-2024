void rom_transaction_end(bool commit)
{
    Rom *rom;
    Rom *tmp;

    QTAILQ_FOREACH_SAFE(rom, &roms, next, tmp) {
        if (rom->committed) {
            continue;
        }
        if (commit) {
            rom->committed = true;
        } else {
            QTAILQ_REMOVE(&roms, rom, next);
            rom_free(rom);
        }
    }
}