static void virtio_scsi_parse_req(VirtIOSCSI *s, VirtQueue *vq,
                                  VirtIOSCSIReq *req)
{
    assert(req->elem.in_num);
    req->vq = vq;
    req->dev = s;
    req->sreq = NULL;
    if (req->elem.out_num) {
        req->req.buf = req->elem.out_sg[0].iov_base;
    }
    req->resp.buf = req->elem.in_sg[0].iov_base;

    if (req->elem.out_num > 1) {
        qemu_sgl_init_external(req, &req->elem.out_sg[1],
                               &req->elem.out_addr[1],
                               req->elem.out_num - 1);
    } else {
        qemu_sgl_init_external(req, &req->elem.in_sg[1],
                               &req->elem.in_addr[1],
                               req->elem.in_num - 1);
    }
}