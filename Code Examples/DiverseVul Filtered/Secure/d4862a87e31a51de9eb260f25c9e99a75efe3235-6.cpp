static void pit_irq_control(void *opaque, int n, int enable)
{
    PITCommonState *pit = opaque;
    PITChannelState *s = &pit->channels[0];

    if (enable) {
        s->irq_disabled = 0;
        pit_irq_timer_update(s, qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL));
    } else {
        s->irq_disabled = 1;
        timer_del(s->irq_timer);
    }
}