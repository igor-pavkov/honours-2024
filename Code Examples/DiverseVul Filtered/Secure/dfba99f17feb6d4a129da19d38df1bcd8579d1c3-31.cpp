void sdhci_common_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    set_bit(DEVICE_CATEGORY_STORAGE, dc->categories);
    dc->vmsd = &sdhci_vmstate;
    dc->reset = sdhci_poweron_reset;
}