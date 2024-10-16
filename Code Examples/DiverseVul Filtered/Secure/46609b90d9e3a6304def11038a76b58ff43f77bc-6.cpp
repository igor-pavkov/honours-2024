static void send_seek(int cyl)
{
    int drive = 0;
    int head = 0;

    floppy_send(CMD_SEEK);
    floppy_send(head << 2 | drive);
    g_assert(!get_irq(FLOPPY_IRQ));
    floppy_send(cyl);
    ack_irq(NULL);
}