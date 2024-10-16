static void sdhci_sysbus_realize(DeviceState *dev, Error **errp)
{
    ERRP_GUARD();
    SDHCIState *s = SYSBUS_SDHCI(dev);
    SysBusDevice *sbd = SYS_BUS_DEVICE(dev);

    sdhci_common_realize(s, errp);
    if (*errp) {
        return;
    }

    if (s->dma_mr) {
        s->dma_as = &s->sysbus_dma_as;
        address_space_init(s->dma_as, s->dma_mr, "sdhci-dma");
    } else {
        /* use system_memory() if property "dma" not set */
        s->dma_as = &address_space_memory;
    }

    sysbus_init_irq(sbd, &s->irq);

    sysbus_init_mmio(sbd, &s->iomem);
}