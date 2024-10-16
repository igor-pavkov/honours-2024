void rom_transaction_begin(void)
{
    Rom *rom;

    /* Ignore ROMs added without the transaction API */
    QTAILQ_FOREACH(rom, &roms, next) {
        rom->committed = true;
    }
}