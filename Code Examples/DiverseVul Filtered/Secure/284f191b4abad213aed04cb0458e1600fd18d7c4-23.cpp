static int query_srq(PVRDMADev *dev, union pvrdma_cmd_req *req,
                     union pvrdma_cmd_resp *rsp)
{
    struct pvrdma_cmd_query_srq *cmd = &req->query_srq;
    struct pvrdma_cmd_query_srq_resp *resp = &rsp->query_srq_resp;

    memset(resp, 0, sizeof(*resp));

    return rdma_rm_query_srq(&dev->rdma_dev_res, cmd->srq_handle,
                             (struct ibv_srq_attr *)&resp->attrs);
}