static int destroy_mr(PVRDMADev *dev, union pvrdma_cmd_req *req,
                      union pvrdma_cmd_resp *rsp)
{
    struct pvrdma_cmd_destroy_mr *cmd = &req->destroy_mr;

    rdma_rm_dealloc_mr(&dev->rdma_dev_res, cmd->mr_handle);

    return 0;
}