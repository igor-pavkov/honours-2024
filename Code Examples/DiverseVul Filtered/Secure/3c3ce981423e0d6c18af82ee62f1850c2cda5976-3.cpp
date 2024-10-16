static void virtio_scsi_save(QEMUFile *f, void *opaque)
{
    VirtIODevice *vdev = VIRTIO_DEVICE(opaque);
    virtio_save(vdev, f);
}