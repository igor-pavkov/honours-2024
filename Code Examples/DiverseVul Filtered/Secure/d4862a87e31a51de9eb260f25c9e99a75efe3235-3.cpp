static void pit_set_channel_gate(PITCommonState *s, PITChannelState *sc,
                                 int val)
{
    switch (sc->mode) {
    default:
    case 0:
    case 4:
        /* XXX: just disable/enable counting */
        break;
    case 1:
    case 5:
        if (sc->gate < val) {
            /* restart counting on rising edge */
            sc->count_load_time = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
            pit_irq_timer_update(sc, sc->count_load_time);
        }
        break;
    case 2:
    case 3:
        if (sc->gate < val) {
            /* restart counting on rising edge */
            sc->count_load_time = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
            pit_irq_timer_update(sc, sc->count_load_time);
        }
        /* XXX: disable/enable counting */
        break;
    }
    sc->gate = val;
}