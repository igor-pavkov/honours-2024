static void test_read_without_media(void)
{
    uint8_t ret;

    ret = send_read_command(CMD_READ);
    g_assert(ret == 0);
}