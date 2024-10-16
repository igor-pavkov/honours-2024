static uint8_t floppy_recv(void)
{
    uint8_t msr;

    msr = inb(FLOPPY_BASE + reg_msr);
    assert_bit_set(msr, RQM | DIO);

    return inb(FLOPPY_BASE + reg_fifo);
}