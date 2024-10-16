static void virtio_scsi_reset(VirtIODevice *vdev)
{
    VirtIOSCSI *s = VIRTIO_SCSI(vdev);
    VirtIOSCSICommon *vs = VIRTIO_SCSI_COMMON(vdev);

    s->resetting++;
    qbus_reset_all(&s->bus.qbus);
    s->resetting--;

    vs->sense_size = VIRTIO_SCSI_SENSE_SIZE;
    vs->cdb_size = VIRTIO_SCSI_CDB_SIZE;
    s->events_dropped = false;
}