static bool cmd_verify(IDEState *s, uint8_t cmd)
{
    bool lba48 = (cmd == WIN_VERIFY_EXT);

    /* do sector number check ? */
    ide_cmd_lba48_transform(s, lba48);

    return true;
}