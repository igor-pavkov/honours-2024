static void imx_usdhc_init(Object *obj)
{
    SDHCIState *s = SYSBUS_SDHCI(obj);

    s->io_ops = &usdhc_mmio_ops;
    s->quirks = SDHCI_QUIRK_NO_BUSY_IRQ;
}