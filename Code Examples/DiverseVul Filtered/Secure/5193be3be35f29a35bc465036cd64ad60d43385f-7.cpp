static void tsc210x_write(TSC210xState *s, uint16_t value)
{
    /*
     * This is a two-state state machine for reading
     * command and data every second time.
     */
    if (!s->state) {
        s->command = value >> 15;
        s->page = (value >> 11) & 0x0f;
        s->offset = (value >> 5) & 0x3f;
        s->state = 1;
    } else {
        if (s->command)
            fprintf(stderr, "tsc210x_write: SPI overrun!\n");
        else
            switch (s->page) {
            case TSC_DATA_REGISTERS_PAGE:
                tsc2102_data_register_write(s, s->offset, value);
                break;
            case TSC_CONTROL_REGISTERS_PAGE:
                tsc2102_control_register_write(s, s->offset, value);
                break;
            case TSC_AUDIO_REGISTERS_PAGE:
                tsc2102_audio_register_write(s, s->offset, value);
                break;
            default:
                hw_error("tsc210x_write: wrong memory page\n");
            }

        tsc210x_pin_update(s);
        s->state = 0;
    }
}