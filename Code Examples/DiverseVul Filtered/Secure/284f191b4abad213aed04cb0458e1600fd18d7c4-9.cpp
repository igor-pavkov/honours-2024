static int destroy_uc(PVRDMADev *dev, union pvrdma_cmd_req *req,
                      union pvrdma_cmd_resp *rsp)
{
    struct pvrdma_cmd_destroy_uc *cmd = &req->destroy_uc;

    rdma_rm_dealloc_uc(&dev->rdma_dev_res, cmd->ctx_handle);

    return 0;
}