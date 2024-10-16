static void sdhci_sysbus_unrealize(DeviceState *dev)
{
    SDHCIState *s = SYSBUS_SDHCI(dev);

    sdhci_common_unrealize(s);

     if (s->dma_mr) {
        address_space_destroy(s->dma_as);
    }
}