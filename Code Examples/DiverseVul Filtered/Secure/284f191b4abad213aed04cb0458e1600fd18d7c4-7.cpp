static int create_uc(PVRDMADev *dev, union pvrdma_cmd_req *req,
                     union pvrdma_cmd_resp *rsp)
{
    struct pvrdma_cmd_create_uc *cmd = &req->create_uc;
    struct pvrdma_cmd_create_uc_resp *resp = &rsp->create_uc_resp;
    int rc;

    memset(resp, 0, sizeof(*resp));
    rc = rdma_rm_alloc_uc(&dev->rdma_dev_res, cmd->pfn, &resp->ctx_handle);

    return rc;
}