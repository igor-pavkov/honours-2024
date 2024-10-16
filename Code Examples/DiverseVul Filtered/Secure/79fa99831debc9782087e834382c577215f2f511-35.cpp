HDACodecDevice *hda_codec_find(HDACodecBus *bus, uint32_t cad)
{
    BusChild *kid;
    HDACodecDevice *cdev;

    QTAILQ_FOREACH(kid, &bus->qbus.children, sibling) {
        DeviceState *qdev = kid->child;
        cdev = HDA_CODEC_DEVICE(qdev);
        if (cdev->cad == cad) {
            return cdev;
        }
    }
    return NULL;
}