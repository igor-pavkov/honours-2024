static void sdhci_bus_class_init(ObjectClass *klass, void *data)
{
    SDBusClass *sbc = SD_BUS_CLASS(klass);

    sbc->set_inserted = sdhci_set_inserted;
    sbc->set_readonly = sdhci_set_readonly;
}