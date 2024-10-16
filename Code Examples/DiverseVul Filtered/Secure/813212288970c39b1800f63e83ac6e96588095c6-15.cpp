static void cmd_mechanism_status(IDEState *s, uint8_t* buf)
{
    int max_len = lduw_be_p(buf + 8);

    stw_be_p(buf, 0);
    /* no current LBA */
    buf[2] = 0;
    buf[3] = 0;
    buf[4] = 0;
    buf[5] = 1;
    stw_be_p(buf + 6, 0);
    ide_atapi_cmd_reply(s, 8, max_len);
}