void sdhci_initfn(SDHCIState *s)
{
    qbus_create_inplace(&s->sdbus, sizeof(s->sdbus),
                        TYPE_SDHCI_BUS, DEVICE(s), "sd-bus");

    s->insert_timer = timer_new_ns(QEMU_CLOCK_VIRTUAL, sdhci_raise_insertion_irq, s);
    s->transfer_timer = timer_new_ns(QEMU_CLOCK_VIRTUAL, sdhci_data_transfer, s);

    s->io_ops = &sdhci_mmio_ops;
}