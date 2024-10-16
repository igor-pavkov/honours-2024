static void sdhci_s3c_init(Object *obj)
{
    SDHCIState *s = SYSBUS_SDHCI(obj);

    s->io_ops = &sdhci_s3c_mmio_ops;
}