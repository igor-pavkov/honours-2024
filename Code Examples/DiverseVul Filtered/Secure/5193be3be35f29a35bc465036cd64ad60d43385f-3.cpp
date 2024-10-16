static void tsc2102_control_register_write(
                TSC210xState *s, int reg, uint16_t value)
{
    switch (reg) {
    case 0x00:	/* TSC ADC */
        s->host_mode = value >> 15;
        s->enabled = !(value & 0x4000);
        if (s->busy && !s->enabled)
            timer_del(s->timer);
        s->busy &= s->enabled;
        s->nextfunction = (value >> 10) & 0xf;
        s->nextprecision = (value >> 8) & 3;
        s->filter = value & 0xff;
        return;

    case 0x01:	/* Status / Keypad Control */
        if ((s->model & 0xff00) == 0x2100)
            s->pin_func = value >> 14;
	else {
            s->kb.scan = (value >> 14) & 1;
            s->kb.debounce = (value >> 11) & 7;
            if (s->kb.intr && s->kb.scan) {
                s->kb.intr = 0;
                qemu_irq_raise(s->kbint);
            }
        }
        return;

    case 0x02:	/* DAC Control */
        if ((s->model & 0xff00) == 0x2300) {
            s->dac_power &= 0x7fff;
            s->dac_power |= 0x8000 & value;
        } else
            goto bad_reg;
        break;

    case 0x03:	/* Reference */
        s->ref = value & 0x1f;
        return;

    case 0x04:	/* Reset */
        if (value == 0xbb00) {
            if (s->busy)
                timer_del(s->timer);
            tsc210x_reset(s);
#ifdef TSC_VERBOSE
        } else {
            fprintf(stderr, "tsc2102_control_register_write: "
                            "wrong value written into RESET\n");
#endif
        }
        return;

    case 0x05:	/* Configuration */
        s->timing = value & 0x3f;
#ifdef TSC_VERBOSE
        if (value & ~0x3f)
            fprintf(stderr, "tsc2102_control_register_write: "
                            "wrong value written into CONFIG\n");
#endif
        return;

    case 0x06:	/* Secondary configuration */
        if ((s->model & 0xff00) == 0x2100)
            goto bad_reg;
        s->kb.mode = value >> 14;
        s->pll[2] = value & 0x3ffff;
        return;

    case 0x10:	/* Keypad Mask */
        if ((s->model & 0xff00) == 0x2100)
            goto bad_reg;
        s->kb.mask = value;
        return;

    default:
    bad_reg:
#ifdef TSC_VERBOSE
        fprintf(stderr, "tsc2102_control_register_write: "
                        "no such register: 0x%02x\n", reg);
#endif
    }
}