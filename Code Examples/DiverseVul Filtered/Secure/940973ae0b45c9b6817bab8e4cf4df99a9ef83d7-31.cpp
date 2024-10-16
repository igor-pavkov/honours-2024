static bool cmd_read_native_max(IDEState *s, uint8_t cmd)
{
    bool lba48 = (cmd == WIN_READ_NATIVE_MAX_EXT);

    /* Refuse if no sectors are addressable (e.g. medium not inserted) */
    if (s->nb_sectors == 0) {
        ide_abort_command(s);
        return true;
    }

    ide_cmd_lba48_transform(s, lba48);
    ide_set_sector(s, s->nb_sectors - 1);

    return true;
}