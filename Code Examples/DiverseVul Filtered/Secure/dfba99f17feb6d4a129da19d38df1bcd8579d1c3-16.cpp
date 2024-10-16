static void sdhci_poweron_reset(DeviceState *dev)
{
    /* QOM (ie power-on) reset. This is identical to reset
     * commanded via device register apart from handling of the
     * 'pending insert on powerup' quirk.
     */
    SDHCIState *s = (SDHCIState *)dev;

    sdhci_reset(s);

    if (s->pending_insert_quirk) {
        s->pending_insert_state = true;
    }
}