static void pit_reset(DeviceState *dev)
{
    PITCommonState *pit = PIT_COMMON(dev);
    PITChannelState *s;

    pit_reset_common(pit);

    s = &pit->channels[0];
    if (!s->irq_disabled) {
        timer_mod(s->irq_timer, s->next_transition_time);
    }
}