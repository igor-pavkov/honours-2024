static int destroy_cq(PVRDMADev *dev, union pvrdma_cmd_req *req,
                      union pvrdma_cmd_resp *rsp)
{
    struct pvrdma_cmd_destroy_cq *cmd = &req->destroy_cq;
    RdmaRmCQ *cq;
    PvrdmaRing *ring;

    cq = rdma_rm_get_cq(&dev->rdma_dev_res, cmd->cq_handle);
    if (!cq) {
        rdma_error_report("Got invalid CQ handle");
        return -EINVAL;
    }

    ring = (PvrdmaRing *)cq->opaque;
    destroy_cq_ring(ring);

    rdma_rm_dealloc_cq(&dev->rdma_dev_res, cmd->cq_handle);

    return 0;
}