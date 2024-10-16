static void sdhci_reset(SDHCIState *s)
{
    DeviceState *dev = DEVICE(s);

    timer_del(s->insert_timer);
    timer_del(s->transfer_timer);

    /* Set all registers to 0. Capabilities/Version registers are not cleared
     * and assumed to always preserve their value, given to them during
     * initialization */
    memset(&s->sdmasysad, 0, (uintptr_t)&s->capareg - (uintptr_t)&s->sdmasysad);

    /* Reset other state based on current card insertion/readonly status */
    sdhci_set_inserted(dev, sdbus_get_inserted(&s->sdbus));
    sdhci_set_readonly(dev, sdbus_get_readonly(&s->sdbus));

    s->data_count = 0;
    s->stopped_state = sdhc_not_stopped;
    s->pending_insert_state = false;
}