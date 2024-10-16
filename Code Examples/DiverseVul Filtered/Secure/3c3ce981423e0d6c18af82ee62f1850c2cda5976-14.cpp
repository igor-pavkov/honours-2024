static int virtio_scsi_load(QEMUFile *f, void *opaque, int version_id)
{
    VirtIODevice *vdev = VIRTIO_DEVICE(opaque);
    int ret;

    ret = virtio_load(vdev, f);
    if (ret) {
        return ret;
    }
    return 0;
}