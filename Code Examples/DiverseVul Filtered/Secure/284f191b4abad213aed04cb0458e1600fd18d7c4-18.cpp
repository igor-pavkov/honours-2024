int pvrdma_exec_cmd(PVRDMADev *dev)
{
    int err = 0xFFFF;
    DSRInfo *dsr_info;

    dsr_info = &dev->dsr_info;

    if (dsr_info->req->hdr.cmd >= sizeof(cmd_handlers) /
                      sizeof(struct cmd_handler)) {
        rdma_error_report("Unsupported command");
        goto out;
    }

    if (!cmd_handlers[dsr_info->req->hdr.cmd].exec) {
        rdma_error_report("Unsupported command (not implemented yet)");
        goto out;
    }

    err = cmd_handlers[dsr_info->req->hdr.cmd].exec(dev, dsr_info->req,
                                                    dsr_info->rsp);
    dsr_info->rsp->hdr.response = dsr_info->req->hdr.response;
    dsr_info->rsp->hdr.ack = cmd_handlers[dsr_info->req->hdr.cmd].ack;
    dsr_info->rsp->hdr.err = err < 0 ? -err : 0;

    trace_pvrdma_exec_cmd(dsr_info->req->hdr.cmd, dsr_info->rsp->hdr.err);

    dev->stats.commands++;

out:
    set_reg_val(dev, PVRDMA_REG_ERR, err);
    post_interrupt(dev, INTR_VEC_CMD_RING);

    return (err == 0) ? 0 : -EINVAL;
}