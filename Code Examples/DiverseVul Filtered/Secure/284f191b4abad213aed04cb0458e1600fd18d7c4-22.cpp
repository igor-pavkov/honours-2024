static int destroy_pd(PVRDMADev *dev, union pvrdma_cmd_req *req,
                      union pvrdma_cmd_resp *rsp)
{
    struct pvrdma_cmd_destroy_pd *cmd = &req->destroy_pd;

    rdma_rm_dealloc_pd(&dev->rdma_dev_res, cmd->pd_handle);

    return 0;
}