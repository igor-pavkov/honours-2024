static bool sdhci_update_irq(SDHCIState *s)
{
    bool pending = sdhci_slotint(s);

    qemu_set_irq(s->irq, pending);

    return pending;
}