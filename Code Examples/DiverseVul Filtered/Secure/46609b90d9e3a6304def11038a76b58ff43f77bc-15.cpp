static uint8_t send_read_no_dma_command(int nb_sect, uint8_t expected_st0)
{
    uint8_t drive = 0;
    uint8_t head = 0;
    uint8_t cyl = 0;
    uint8_t sect_addr = 1;
    uint8_t sect_size = 2;
    uint8_t eot = nb_sect;
    uint8_t gap = 0x1b;
    uint8_t gpl = 0xff;

    uint8_t msr = 0;
    uint8_t st0;

    uint8_t ret = 0;

    floppy_send(CMD_READ);
    floppy_send(head << 2 | drive);
    g_assert(!get_irq(FLOPPY_IRQ));
    floppy_send(cyl);
    floppy_send(head);
    floppy_send(sect_addr);
    floppy_send(sect_size);
    floppy_send(eot);
    floppy_send(gap);
    floppy_send(gpl);

    uint16_t i = 0;
    uint8_t n = 2;
    for (; i < n; i++) {
        msr = inb(FLOPPY_BASE + reg_msr);
        if (msr == (BUSY | NONDMA | DIO | RQM)) {
            break;
        }
        sleep(1);
    }

    if (i >= n) {
        return 1;
    }

    /* Non-DMA mode */
    for (i = 0; i < 512 * 2 * nb_sect; i++) {
        msr = inb(FLOPPY_BASE + reg_msr);
        assert_bit_set(msr, BUSY | RQM | DIO);
        inb(FLOPPY_BASE + reg_fifo);
    }

    msr = inb(FLOPPY_BASE + reg_msr);
    assert_bit_set(msr, BUSY | RQM | DIO);
    g_assert(get_irq(FLOPPY_IRQ));

    st0 = floppy_recv();
    if (st0 != expected_st0) {
        ret = 1;
    }

    floppy_recv();
    floppy_recv();
    floppy_recv();
    floppy_recv();
    floppy_recv();
    g_assert(get_irq(FLOPPY_IRQ));
    floppy_recv();

    /* Check that we're back in command phase */
    msr = inb(FLOPPY_BASE + reg_msr);
    assert_bit_clear(msr, BUSY | DIO);
    assert_bit_set(msr, RQM);
    g_assert(!get_irq(FLOPPY_IRQ));

    return ret;
}