static uint16_t tsc2102_data_register_read(TSC210xState *s, int reg)
{
    switch (reg) {
    case 0x00:	/* X */
        s->dav &= 0xfbff;
        return TSC_CUT_RESOLUTION(X_TRANSFORM(s), s->precision) +
                (s->noise & 3);

    case 0x01:	/* Y */
        s->noise ++;
        s->dav &= 0xfdff;
        return TSC_CUT_RESOLUTION(Y_TRANSFORM(s), s->precision) ^
                (s->noise & 3);

    case 0x02:	/* Z1 */
        s->dav &= 0xfeff;
        return TSC_CUT_RESOLUTION(Z1_TRANSFORM(s), s->precision) -
                (s->noise & 3);

    case 0x03:	/* Z2 */
        s->dav &= 0xff7f;
        return TSC_CUT_RESOLUTION(Z2_TRANSFORM(s), s->precision) |
                (s->noise & 3);

    case 0x04:	/* KPData */
        if ((s->model & 0xff00) == 0x2300) {
            if (s->kb.intr && (s->kb.mode & 2)) {
                s->kb.intr = 0;
                qemu_irq_raise(s->kbint);
            }
            return s->kb.down;
        }

        return 0xffff;

    case 0x05:	/* BAT1 */
        s->dav &= 0xffbf;
        return TSC_CUT_RESOLUTION(BAT1_VAL, s->precision) +
                (s->noise & 6);

    case 0x06:	/* BAT2 */
        s->dav &= 0xffdf;
        return TSC_CUT_RESOLUTION(BAT2_VAL, s->precision);

    case 0x07:	/* AUX1 */
        s->dav &= 0xffef;
        return TSC_CUT_RESOLUTION(AUX1_VAL, s->precision);

    case 0x08:	/* AUX2 */
        s->dav &= 0xfff7;
        return 0xffff;

    case 0x09:	/* TEMP1 */
        s->dav &= 0xfffb;
        return TSC_CUT_RESOLUTION(TEMP1_VAL, s->precision) -
                (s->noise & 5);

    case 0x0a:	/* TEMP2 */
        s->dav &= 0xfffd;
        return TSC_CUT_RESOLUTION(TEMP2_VAL, s->precision) ^
                (s->noise & 3);

    case 0x0b:	/* DAC */
        s->dav &= 0xfffe;
        return 0xffff;

    default:
#ifdef TSC_VERBOSE
        fprintf(stderr, "tsc2102_data_register_read: "
                        "no such register: 0x%02x\n", reg);
#endif
        return 0xffff;
    }
}