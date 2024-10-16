static void test_verify(void)
{
    uint8_t ret;

    ret = send_read_command(CMD_VERIFY);
    g_assert(ret == 0);
}