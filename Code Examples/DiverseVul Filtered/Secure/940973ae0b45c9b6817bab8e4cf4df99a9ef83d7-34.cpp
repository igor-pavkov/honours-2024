static bool cmd_cfa_erase_sectors(IDEState *s, uint8_t cmd)
{
    /* WIN_SECURITY_FREEZE_LOCK has the same ID as CFA_WEAR_LEVEL and is
     * required for Windows 8 to work with AHCI */

    if (cmd == CFA_WEAR_LEVEL) {
        s->nsector = 0;
    }

    if (cmd == CFA_ERASE_SECTORS) {
        s->media_changed = 1;
    }

    return true;
}