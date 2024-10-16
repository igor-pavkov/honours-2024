static void client_init_reply_buf(void)
{
    replybuf_pos = replybuf;
    replybuf_left = sizeof replybuf - 1U;
}