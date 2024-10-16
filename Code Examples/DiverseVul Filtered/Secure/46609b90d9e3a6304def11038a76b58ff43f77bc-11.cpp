static void floppy_send(uint8_t byte)
{
    uint8_t msr;

    msr = inb(FLOPPY_BASE + reg_msr);
    assert_bit_set(msr, RQM);
    assert_bit_clear(msr, DIO);

    outb(FLOPPY_BASE + reg_fifo, byte);
}