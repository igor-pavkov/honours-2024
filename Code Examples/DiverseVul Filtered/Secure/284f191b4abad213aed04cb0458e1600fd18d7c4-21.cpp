static int modify_qp(PVRDMADev *dev, union pvrdma_cmd_req *req,
                     union pvrdma_cmd_resp *rsp)
{
    struct pvrdma_cmd_modify_qp *cmd = &req->modify_qp;
    int rc;

    /* No need to verify sgid_index since it is u8 */

    rc = rdma_rm_modify_qp(&dev->rdma_dev_res, &dev->backend_dev,
                           cmd->qp_handle, cmd->attr_mask,
                           cmd->attrs.ah_attr.grh.sgid_index,
                           (union ibv_gid *)&cmd->attrs.ah_attr.grh.dgid,
                           cmd->attrs.dest_qp_num,
                           (enum ibv_qp_state)cmd->attrs.qp_state,
                           cmd->attrs.qkey, cmd->attrs.rq_psn,
                           cmd->attrs.sq_psn);

    return rc;
}