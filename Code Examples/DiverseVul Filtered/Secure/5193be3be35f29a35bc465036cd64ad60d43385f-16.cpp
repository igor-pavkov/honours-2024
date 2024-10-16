static void tsc210x_pin_update(TSC210xState *s)
{
    int64_t expires;
    int pin_state;

    switch (s->pin_func) {
    case 0:
        pin_state = s->pressure;
        break;
    case 1:
        pin_state = !!s->dav;
        break;
    case 2:
    default:
        pin_state = s->pressure && !s->dav;
    }

    if (!s->enabled)
        pin_state = 0;

    if (pin_state != s->irq) {
        s->irq = pin_state;
        qemu_set_irq(s->pint, !s->irq);
    }

    switch (s->nextfunction) {
    case TSC_MODE_XY_SCAN:
    case TSC_MODE_XYZ_SCAN:
        if (!s->pressure)
            return;
        break;

    case TSC_MODE_X:
    case TSC_MODE_Y:
    case TSC_MODE_Z:
        if (!s->pressure)
            return;
        /* Fall through */
    case TSC_MODE_BAT1:
    case TSC_MODE_BAT2:
    case TSC_MODE_AUX:
    case TSC_MODE_TEMP1:
    case TSC_MODE_TEMP2:
        if (s->dav)
            s->enabled = 0;
        break;

    case TSC_MODE_AUX_SCAN:
    case TSC_MODE_PORT_SCAN:
        break;

    case TSC_MODE_NO_SCAN:
    case TSC_MODE_XX_DRV:
    case TSC_MODE_YY_DRV:
    case TSC_MODE_YX_DRV:
    default:
        return;
    }

    if (!s->enabled || s->busy || s->dav)
        return;

    s->busy = 1;
    s->precision = s->nextprecision;
    s->function = s->nextfunction;
    expires = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) + (get_ticks_per_sec() >> 10);
    timer_mod(s->timer, expires);
}