static void cmd_read(IDEState *s, uint8_t* buf)
{
    int nb_sectors, lba;

    if (buf[0] == GPCMD_READ_10) {
        nb_sectors = lduw_be_p(buf + 7);
    } else {
        nb_sectors = ldl_be_p(buf + 6);
    }

    lba = ldl_be_p(buf + 2);
    if (nb_sectors == 0) {
        ide_atapi_cmd_ok(s);
        return;
    }

    ide_atapi_cmd_read(s, lba, nb_sectors, 2048);
}