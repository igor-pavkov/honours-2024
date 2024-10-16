static inline void pit_load_count(PITChannelState *s, int val)
{
    if (val == 0)
        val = 0x10000;
    s->count_load_time = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
    s->count = val;
    pit_irq_timer_update(s, s->count_load_time);
}