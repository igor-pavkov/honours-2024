static int destroy_srq(PVRDMADev *dev, union pvrdma_cmd_req *req,
                       union pvrdma_cmd_resp *rsp)
{
    struct pvrdma_cmd_destroy_srq *cmd = &req->destroy_srq;
    RdmaRmSRQ *srq;
    PvrdmaRing *ring;

    srq = rdma_rm_get_srq(&dev->rdma_dev_res, cmd->srq_handle);
    if (!srq) {
        return -EINVAL;
    }

    ring = (PvrdmaRing *)srq->opaque;
    destroy_srq_ring(ring);
    rdma_rm_dealloc_srq(&dev->rdma_dev_res, cmd->srq_handle);

    return 0;
}