static void test_cmos(void)
{
    uint8_t cmos;

    cmos = cmos_read(CMOS_FLOPPY);
    g_assert(cmos == 0x40 || cmos == 0x50);
}