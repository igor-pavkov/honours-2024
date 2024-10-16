static void virtio_scsi_command_complete(SCSIRequest *r, uint32_t status,
                                         size_t resid)
{
    VirtIOSCSIReq *req = r->hba_private;
    VirtIOSCSI *s = req->dev;
    VirtIOSCSICommon *vs = VIRTIO_SCSI_COMMON(s);
    uint32_t sense_len;

    if (r->io_canceled) {
        return;
    }

    req->resp.cmd->response = VIRTIO_SCSI_S_OK;
    req->resp.cmd->status = status;
    if (req->resp.cmd->status == GOOD) {
        req->resp.cmd->resid = tswap32(resid);
    } else {
        req->resp.cmd->resid = 0;
        sense_len = scsi_req_get_sense(r, req->resp.cmd->sense,
                                       vs->sense_size);
        req->resp.cmd->sense_len = tswap32(sense_len);
    }
    virtio_scsi_complete_req(req);
}