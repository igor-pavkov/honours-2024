static void sdhci_sysbus_init(Object *obj)
{
    SDHCIState *s = SYSBUS_SDHCI(obj);

    sdhci_initfn(s);
}