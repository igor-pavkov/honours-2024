static int create_mr(PVRDMADev *dev, union pvrdma_cmd_req *req,
                     union pvrdma_cmd_resp *rsp)
{
    struct pvrdma_cmd_create_mr *cmd = &req->create_mr;
    struct pvrdma_cmd_create_mr_resp *resp = &rsp->create_mr_resp;
    PCIDevice *pci_dev = PCI_DEVICE(dev);
    void *host_virt = NULL;
    int rc = 0;

    memset(resp, 0, sizeof(*resp));

    if (!(cmd->flags & PVRDMA_MR_FLAG_DMA)) {
        host_virt = pvrdma_map_to_pdir(pci_dev, cmd->pdir_dma, cmd->nchunks,
                                       cmd->length);
        if (!host_virt) {
            rdma_error_report("Failed to map to pdir");
            return -EINVAL;
        }
    }

    rc = rdma_rm_alloc_mr(&dev->rdma_dev_res, cmd->pd_handle, cmd->start,
                          cmd->length, host_virt, cmd->access_flags,
                          &resp->mr_handle, &resp->lkey, &resp->rkey);
    if (rc && host_virt) {
        munmap(host_virt, cmd->length);
    }

    return rc;
}