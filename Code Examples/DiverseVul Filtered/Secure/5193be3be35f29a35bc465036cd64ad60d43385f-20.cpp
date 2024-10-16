static uint16_t tsc210x_read(TSC210xState *s)
{
    uint16_t ret = 0x0000;

    if (!s->command)
        fprintf(stderr, "tsc210x_read: SPI underrun!\n");

    switch (s->page) {
    case TSC_DATA_REGISTERS_PAGE:
        ret = tsc2102_data_register_read(s, s->offset);
        if (!s->dav)
            qemu_irq_raise(s->davint);
        break;
    case TSC_CONTROL_REGISTERS_PAGE:
        ret = tsc2102_control_register_read(s, s->offset);
        break;
    case TSC_AUDIO_REGISTERS_PAGE:
        ret = tsc2102_audio_register_read(s, s->offset);
        break;
    default:
        hw_error("tsc210x_read: wrong memory page\n");
    }

    tsc210x_pin_update(s);

    /* Allow sequential reads.  */
    s->offset ++;
    s->state = 0;
    return ret;
}