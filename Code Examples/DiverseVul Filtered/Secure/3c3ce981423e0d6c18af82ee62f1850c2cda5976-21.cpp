static void virtio_scsi_complete_req(VirtIOSCSIReq *req)
{
    VirtIOSCSI *s = req->dev;
    VirtQueue *vq = req->vq;
    VirtIODevice *vdev = VIRTIO_DEVICE(s);
    virtqueue_push(vq, &req->elem, req->qsgl.size + req->elem.in_sg[0].iov_len);
    qemu_sglist_destroy(&req->qsgl);
    if (req->sreq) {
        req->sreq->hba_private = NULL;
        scsi_req_unref(req->sreq);
    }
    g_free(req);
    virtio_notify(vdev, vq);
}