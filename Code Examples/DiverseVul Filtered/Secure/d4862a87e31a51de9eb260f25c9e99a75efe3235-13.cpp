static void pit_irq_timer_update(PITChannelState *s, int64_t current_time)
{
    int64_t expire_time;
    int irq_level;

    if (!s->irq_timer || s->irq_disabled) {
        return;
    }
    expire_time = pit_get_next_transition_time(s, current_time);
    irq_level = pit_get_out(s, current_time);
    qemu_set_irq(s->irq, irq_level);
#ifdef DEBUG_PIT
    printf("irq_level=%d next_delay=%f\n",
           irq_level,
           (double)(expire_time - current_time) / get_ticks_per_sec());
#endif
    s->next_transition_time = expire_time;
    if (expire_time != -1)
        timer_mod(s->irq_timer, expire_time);
    else
        timer_del(s->irq_timer);
}