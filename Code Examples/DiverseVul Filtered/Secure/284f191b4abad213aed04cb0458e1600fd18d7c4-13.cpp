static int create_cq(PVRDMADev *dev, union pvrdma_cmd_req *req,
                     union pvrdma_cmd_resp *rsp)
{
    struct pvrdma_cmd_create_cq *cmd = &req->create_cq;
    struct pvrdma_cmd_create_cq_resp *resp = &rsp->create_cq_resp;
    PvrdmaRing *ring = NULL;
    int rc;

    memset(resp, 0, sizeof(*resp));

    resp->cqe = cmd->cqe;

    rc = create_cq_ring(PCI_DEVICE(dev), &ring, cmd->pdir_dma, cmd->nchunks,
                        cmd->cqe);
    if (rc) {
        return rc;
    }

    rc = rdma_rm_alloc_cq(&dev->rdma_dev_res, &dev->backend_dev, cmd->cqe,
                          &resp->cq_handle, ring);
    if (rc) {
        destroy_cq_ring(ring);
    }

    resp->cqe = cmd->cqe;

    return rc;
}