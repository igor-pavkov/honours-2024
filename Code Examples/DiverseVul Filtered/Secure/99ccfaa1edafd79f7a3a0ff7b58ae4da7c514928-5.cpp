static void pcnet_poll_timer(void *opaque)
{
    PCNetState *s = opaque;

    timer_del(s->poll_timer);

    if (CSR_TDMD(s)) {
        pcnet_transmit(s);
    }

    pcnet_update_irq(s);

    if (!CSR_STOP(s) && !CSR_SPND(s) && !CSR_DPOLL(s)) {
        uint64_t now = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) * 33;
        if (!s->timer || !now) {
            s->timer = now;
        } else {
            uint64_t t = now - s->timer + CSR_POLL(s);
            if (t > 0xffffLL) {
                pcnet_poll(s);
                CSR_POLL(s) = CSR_PINT(s);
            } else {
                CSR_POLL(s) = t;
            }
        }
        timer_mod(s->poll_timer,
            pcnet_get_next_poll_time(s,qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL)));
    }
}