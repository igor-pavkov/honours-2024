static int create_srq(PVRDMADev *dev, union pvrdma_cmd_req *req,
                      union pvrdma_cmd_resp *rsp)
{
    struct pvrdma_cmd_create_srq *cmd = &req->create_srq;
    struct pvrdma_cmd_create_srq_resp *resp = &rsp->create_srq_resp;
    PvrdmaRing *ring = NULL;
    int rc;

    memset(resp, 0, sizeof(*resp));

    rc = create_srq_ring(PCI_DEVICE(dev), &ring, cmd->pdir_dma,
                         cmd->attrs.max_wr, cmd->attrs.max_sge,
                         cmd->nchunks);
    if (rc) {
        return rc;
    }

    rc = rdma_rm_alloc_srq(&dev->rdma_dev_res, cmd->pd_handle,
                           cmd->attrs.max_wr, cmd->attrs.max_sge,
                           cmd->attrs.srq_limit, &resp->srqn, ring);
    if (rc) {
        destroy_srq_ring(ring);
        return rc;
    }

    return 0;
}