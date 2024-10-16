static void test_sense_interrupt(void)
{
    int drive = 0;
    int head = 0;
    int cyl = 0;
    int ret = 0;

    floppy_send(CMD_SENSE_INT);
    ret = floppy_recv();
    g_assert(ret == 0x80);

    floppy_send(CMD_SEEK);
    floppy_send(head << 2 | drive);
    g_assert(!get_irq(FLOPPY_IRQ));
    floppy_send(cyl);

    floppy_send(CMD_SENSE_INT);
    ret = floppy_recv();
    g_assert(ret == 0x20);
    floppy_recv();
}