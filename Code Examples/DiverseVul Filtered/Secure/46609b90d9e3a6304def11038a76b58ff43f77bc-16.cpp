static void ack_irq(uint8_t *pcn)
{
    uint8_t ret;

    g_assert(get_irq(FLOPPY_IRQ));
    floppy_send(CMD_SENSE_INT);
    floppy_recv();

    ret = floppy_recv();
    if (pcn != NULL) {
        *pcn = ret;
    }

    g_assert(!get_irq(FLOPPY_IRQ));
}