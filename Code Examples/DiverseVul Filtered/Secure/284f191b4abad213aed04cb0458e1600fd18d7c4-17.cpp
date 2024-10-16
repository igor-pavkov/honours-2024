static int destroy_qp(PVRDMADev *dev, union pvrdma_cmd_req *req,
                      union pvrdma_cmd_resp *rsp)
{
    struct pvrdma_cmd_destroy_qp *cmd = &req->destroy_qp;
    RdmaRmQP *qp;
    PvrdmaRing *ring;

    qp = rdma_rm_get_qp(&dev->rdma_dev_res, cmd->qp_handle);
    if (!qp) {
        return -EINVAL;
    }

    ring = (PvrdmaRing *)qp->opaque;
    destroy_qp_rings(ring, qp->is_srq);
    rdma_rm_dealloc_qp(&dev->rdma_dev_res, cmd->qp_handle);

    return 0;
}