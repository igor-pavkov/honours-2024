static void floppy_drive_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *k = DEVICE_CLASS(klass);
    k->realize = floppy_drive_realize;
    set_bit(DEVICE_CATEGORY_STORAGE, k->categories);
    k->bus_type = TYPE_FLOPPY_BUS;
    device_class_set_props(k, floppy_drive_properties);
    k->desc = "virtual floppy drive";
}