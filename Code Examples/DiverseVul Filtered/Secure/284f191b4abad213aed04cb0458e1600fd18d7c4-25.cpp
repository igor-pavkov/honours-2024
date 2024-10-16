static int query_port(PVRDMADev *dev, union pvrdma_cmd_req *req,
                      union pvrdma_cmd_resp *rsp)
{
    struct pvrdma_cmd_query_port *cmd = &req->query_port;
    struct pvrdma_cmd_query_port_resp *resp = &rsp->query_port_resp;
    struct pvrdma_port_attr attrs = {};

    if (cmd->port_num > MAX_PORTS) {
        return -EINVAL;
    }

    if (rdma_backend_query_port(&dev->backend_dev,
                                (struct ibv_port_attr *)&attrs)) {
        return -ENOMEM;
    }

    memset(resp, 0, sizeof(*resp));

    resp->attrs.state = dev->func0->device_active ? attrs.state :
                                                    PVRDMA_PORT_DOWN;
    resp->attrs.max_mtu = attrs.max_mtu;
    resp->attrs.active_mtu = attrs.active_mtu;
    resp->attrs.phys_state = attrs.phys_state;
    resp->attrs.gid_tbl_len = MIN(MAX_PORT_GIDS, attrs.gid_tbl_len);
    resp->attrs.max_msg_sz = 1024;
    resp->attrs.pkey_tbl_len = MIN(MAX_PORT_PKEYS, attrs.pkey_tbl_len);
    resp->attrs.active_width = 1;
    resp->attrs.active_speed = 1;

    return 0;
}