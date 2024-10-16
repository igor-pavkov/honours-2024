static void hda_codec_dev_realize(DeviceState *qdev, Error **errp)
{
    HDACodecBus *bus = HDA_BUS(qdev->parent_bus);
    HDACodecDevice *dev = HDA_CODEC_DEVICE(qdev);
    HDACodecDeviceClass *cdc = HDA_CODEC_DEVICE_GET_CLASS(dev);

    if (dev->cad == -1) {
        dev->cad = bus->next_cad;
    }
    if (dev->cad >= 15) {
        error_setg(errp, "HDA audio codec address is full");
        return;
    }
    bus->next_cad = dev->cad + 1;
    if (cdc->init(dev) != 0) {
        error_setg(errp, "HDA audio init failed");
    }
}