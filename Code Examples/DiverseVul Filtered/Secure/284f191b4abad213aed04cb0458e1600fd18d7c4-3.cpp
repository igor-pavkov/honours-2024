static int destroy_bind(PVRDMADev *dev, union pvrdma_cmd_req *req,
                        union pvrdma_cmd_resp *rsp)
{
    int rc;

    struct pvrdma_cmd_destroy_bind *cmd = &req->destroy_bind;

    if (cmd->index >= MAX_PORT_GIDS) {
        return -EINVAL;
    }

    rc = rdma_rm_del_gid(&dev->rdma_dev_res, &dev->backend_dev,
                        dev->backend_eth_device_name, cmd->index);

    return rc;
}