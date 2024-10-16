static void sdhci_sysbus_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    device_class_set_props(dc, sdhci_sysbus_properties);
    dc->realize = sdhci_sysbus_realize;
    dc->unrealize = sdhci_sysbus_unrealize;

    sdhci_common_class_init(klass, data);
}