static void fuzz_registers(void)
{
    unsigned int i;

    for (i = 0; i < 1000; i++) {
        uint8_t reg, val;

        reg = (uint8_t)g_test_rand_int_range(0, 8);
        val = (uint8_t)g_test_rand_int_range(0, 256);

        outb(FLOPPY_BASE + reg, val);
        inb(FLOPPY_BASE + reg);
    }
}