static void pit_post_load(PITCommonState *s)
{
    PITChannelState *sc = &s->channels[0];

    if (sc->next_transition_time != -1) {
        timer_mod(sc->irq_timer, sc->next_transition_time);
    } else {
        timer_del(sc->irq_timer);
    }
}