static void virtio_device_class_init(ObjectClass *klass, void *data)
{
    /* Set the default value here. */
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = virtio_device_realize;
    dc->unrealize = virtio_device_unrealize;
    dc->bus_type = TYPE_VIRTIO_BUS;
}