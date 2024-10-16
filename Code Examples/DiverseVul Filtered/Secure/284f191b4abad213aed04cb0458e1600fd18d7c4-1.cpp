static int modify_srq(PVRDMADev *dev, union pvrdma_cmd_req *req,
                      union pvrdma_cmd_resp *rsp)
{
    struct pvrdma_cmd_modify_srq *cmd = &req->modify_srq;

    /* Only support SRQ limit */
    if (!(cmd->attr_mask & IBV_SRQ_LIMIT) ||
        (cmd->attr_mask & IBV_SRQ_MAX_WR))
            return -EINVAL;

    return rdma_rm_modify_srq(&dev->rdma_dev_res, cmd->srq_handle,
                              (struct ibv_srq_attr *)&cmd->attrs,
                              cmd->attr_mask);
}