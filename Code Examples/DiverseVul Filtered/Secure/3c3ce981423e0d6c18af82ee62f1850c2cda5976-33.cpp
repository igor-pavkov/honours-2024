static void virtio_scsi_save_request(QEMUFile *f, SCSIRequest *sreq)
{
    VirtIOSCSIReq *req = sreq->hba_private;
    VirtIOSCSICommon *vs = VIRTIO_SCSI_COMMON(req->dev);
    uint32_t n = virtio_queue_get_id(req->vq) - 2;

    assert(n < vs->conf.num_queues);
    qemu_put_be32s(f, &n);
    qemu_put_buffer(f, (unsigned char *)&req->elem, sizeof(req->elem));
}