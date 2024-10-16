static int create_qp(PVRDMADev *dev, union pvrdma_cmd_req *req,
                     union pvrdma_cmd_resp *rsp)
{
    struct pvrdma_cmd_create_qp *cmd = &req->create_qp;
    struct pvrdma_cmd_create_qp_resp *resp = &rsp->create_qp_resp;
    PvrdmaRing *rings = NULL;
    int rc;

    memset(resp, 0, sizeof(*resp));

    rc = create_qp_rings(PCI_DEVICE(dev), cmd->pdir_dma, &rings,
                         cmd->max_send_wr, cmd->max_send_sge, cmd->send_chunks,
                         cmd->max_recv_wr, cmd->max_recv_sge,
                         cmd->total_chunks - cmd->send_chunks - 1, cmd->is_srq);
    if (rc) {
        return rc;
    }

    rc = rdma_rm_alloc_qp(&dev->rdma_dev_res, cmd->pd_handle, cmd->qp_type,
                          cmd->max_send_wr, cmd->max_send_sge,
                          cmd->send_cq_handle, cmd->max_recv_wr,
                          cmd->max_recv_sge, cmd->recv_cq_handle, rings,
                          &resp->qpn, cmd->is_srq, cmd->srq_handle);
    if (rc) {
        destroy_qp_rings(rings, cmd->is_srq);
        return rc;
    }

    resp->max_send_wr = cmd->max_send_wr;
    resp->max_recv_wr = cmd->max_recv_wr;
    resp->max_send_sge = cmd->max_send_sge;
    resp->max_recv_sge = cmd->max_recv_sge;
    resp->max_inline_data = cmd->max_inline_data;

    return 0;
}