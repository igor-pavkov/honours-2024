static int create_pd(PVRDMADev *dev, union pvrdma_cmd_req *req,
                     union pvrdma_cmd_resp *rsp)
{
    struct pvrdma_cmd_create_pd *cmd = &req->create_pd;
    struct pvrdma_cmd_create_pd_resp *resp = &rsp->create_pd_resp;
    int rc;

    memset(resp, 0, sizeof(*resp));
    rc = rdma_rm_alloc_pd(&dev->rdma_dev_res, &dev->backend_dev,
                          &resp->pd_handle, cmd->ctx_handle);

    return rc;
}