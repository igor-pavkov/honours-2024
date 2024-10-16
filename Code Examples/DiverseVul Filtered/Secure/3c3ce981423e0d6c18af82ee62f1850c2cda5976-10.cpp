void virtio_scsi_common_unrealize(DeviceState *dev, Error **errp)
{
    VirtIODevice *vdev = VIRTIO_DEVICE(dev);
    VirtIOSCSICommon *vs = VIRTIO_SCSI_COMMON(dev);

    g_free(vs->cmd_vqs);
    virtio_cleanup(vdev);
}