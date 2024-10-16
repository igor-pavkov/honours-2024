static int create_bind(PVRDMADev *dev, union pvrdma_cmd_req *req,
                       union pvrdma_cmd_resp *rsp)
{
    struct pvrdma_cmd_create_bind *cmd = &req->create_bind;
    int rc;
    union ibv_gid *gid = (union ibv_gid *)&cmd->new_gid;

    if (cmd->index >= MAX_PORT_GIDS) {
        return -EINVAL;
    }

    rc = rdma_rm_add_gid(&dev->rdma_dev_res, &dev->backend_dev,
                         dev->backend_eth_device_name, gid, cmd->index);

    return rc;
}