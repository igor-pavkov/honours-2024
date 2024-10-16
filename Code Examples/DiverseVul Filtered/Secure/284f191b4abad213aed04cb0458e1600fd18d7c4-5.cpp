static int query_qp(PVRDMADev *dev, union pvrdma_cmd_req *req,
                     union pvrdma_cmd_resp *rsp)
{
    struct pvrdma_cmd_query_qp *cmd = &req->query_qp;
    struct pvrdma_cmd_query_qp_resp *resp = &rsp->query_qp_resp;
    struct ibv_qp_init_attr init_attr;
    int rc;

    memset(resp, 0, sizeof(*resp));

    rc = rdma_rm_query_qp(&dev->rdma_dev_res, &dev->backend_dev, cmd->qp_handle,
                          (struct ibv_qp_attr *)&resp->attrs, cmd->attr_mask,
                          &init_attr);

    return rc;
}