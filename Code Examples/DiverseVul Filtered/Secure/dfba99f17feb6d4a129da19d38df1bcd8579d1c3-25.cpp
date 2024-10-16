static void sdhci_sysbus_finalize(Object *obj)
{
    SDHCIState *s = SYSBUS_SDHCI(obj);

    if (s->dma_mr) {
        object_unparent(OBJECT(s->dma_mr));
    }

    sdhci_uninitfn(s);
}