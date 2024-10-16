static uint16_t tsc2102_control_register_read(
                TSC210xState *s, int reg)
{
    switch (reg) {
    case 0x00:	/* TSC ADC */
        return (s->pressure << 15) | ((!s->busy) << 14) |
                (s->nextfunction << 10) | (s->nextprecision << 8) | s->filter; 

    case 0x01:	/* Status / Keypad Control */
        if ((s->model & 0xff00) == 0x2100)
            return (s->pin_func << 14) | ((!s->enabled) << 13) |
                    (s->host_mode << 12) | ((!!s->dav) << 11) | s->dav;
        else
            return (s->kb.intr << 15) | ((s->kb.scan || !s->kb.down) << 14) |
                    (s->kb.debounce << 11);

    case 0x02:	/* DAC Control */
        if ((s->model & 0xff00) == 0x2300)
            return s->dac_power & 0x8000;
        else
            goto bad_reg;

    case 0x03:	/* Reference */
        return s->ref;

    case 0x04:	/* Reset */
        return 0xffff;

    case 0x05:	/* Configuration */
        return s->timing;

    case 0x06:	/* Secondary configuration */
        if ((s->model & 0xff00) == 0x2100)
            goto bad_reg;
        return ((!s->dav) << 15) | ((s->kb.mode & 1) << 14) | s->pll[2];

    case 0x10:	/* Keypad Mask */
        if ((s->model & 0xff00) == 0x2100)
            goto bad_reg;
        return s->kb.mask;

    default:
    bad_reg:
#ifdef TSC_VERBOSE
        fprintf(stderr, "tsc2102_control_register_read: "
                        "no such register: 0x%02x\n", reg);
#endif
        return 0xffff;
    }
}