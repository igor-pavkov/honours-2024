static void virtio_scsi_handle_ctrl(VirtIODevice *vdev, VirtQueue *vq)
{
    VirtIOSCSI *s = (VirtIOSCSI *)vdev;
    VirtIOSCSIReq *req;

    while ((req = virtio_scsi_pop_req(s, vq))) {
        int out_size, in_size;
        if (req->elem.out_num < 1 || req->elem.in_num < 1) {
            virtio_scsi_bad_req();
            continue;
        }

        out_size = req->elem.out_sg[0].iov_len;
        in_size = req->elem.in_sg[0].iov_len;
        if (req->req.tmf->type == VIRTIO_SCSI_T_TMF) {
            if (out_size < sizeof(VirtIOSCSICtrlTMFReq) ||
                in_size < sizeof(VirtIOSCSICtrlTMFResp)) {
                virtio_scsi_bad_req();
            }
            virtio_scsi_do_tmf(s, req);

        } else if (req->req.tmf->type == VIRTIO_SCSI_T_AN_QUERY ||
                   req->req.tmf->type == VIRTIO_SCSI_T_AN_SUBSCRIBE) {
            if (out_size < sizeof(VirtIOSCSICtrlANReq) ||
                in_size < sizeof(VirtIOSCSICtrlANResp)) {
                virtio_scsi_bad_req();
            }
            req->resp.an->event_actual = 0;
            req->resp.an->response = VIRTIO_SCSI_S_OK;
        }
        virtio_scsi_complete_req(req);
    }
}