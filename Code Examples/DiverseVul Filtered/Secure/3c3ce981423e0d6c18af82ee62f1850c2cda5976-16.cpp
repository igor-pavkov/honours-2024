static void virtio_scsi_device_unrealize(DeviceState *dev, Error **errp)
{
    VirtIOSCSI *s = VIRTIO_SCSI(dev);

    unregister_savevm(dev, "virtio-scsi", s);

    virtio_scsi_common_unrealize(dev, errp);
}