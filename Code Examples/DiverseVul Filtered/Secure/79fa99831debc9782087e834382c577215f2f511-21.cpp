static void intel_hda_notify_codecs(IntelHDAState *d, uint32_t stream, bool running, bool output)
{
    BusChild *kid;
    HDACodecDevice *cdev;

    QTAILQ_FOREACH(kid, &d->codecs.qbus.children, sibling) {
        DeviceState *qdev = kid->child;
        HDACodecDeviceClass *cdc;

        cdev = HDA_CODEC_DEVICE(qdev);
        cdc = HDA_CODEC_DEVICE_GET_CLASS(cdev);
        if (cdc->stream) {
            cdc->stream(cdev, stream, running, output);
        }
    }
}