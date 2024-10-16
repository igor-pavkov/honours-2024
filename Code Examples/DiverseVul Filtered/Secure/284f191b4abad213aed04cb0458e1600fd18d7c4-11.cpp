static int query_pkey(PVRDMADev *dev, union pvrdma_cmd_req *req,
                      union pvrdma_cmd_resp *rsp)
{
    struct pvrdma_cmd_query_pkey *cmd = &req->query_pkey;
    struct pvrdma_cmd_query_pkey_resp *resp = &rsp->query_pkey_resp;

    if (cmd->port_num > MAX_PORTS) {
        return -EINVAL;
    }

    if (cmd->index > MAX_PKEYS) {
        return -EINVAL;
    }

    memset(resp, 0, sizeof(*resp));

    resp->pkey = PVRDMA_PKEY;

    return 0;
}