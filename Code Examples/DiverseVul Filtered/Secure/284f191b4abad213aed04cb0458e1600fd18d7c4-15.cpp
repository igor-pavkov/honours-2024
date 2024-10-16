static int create_srq_ring(PCIDevice *pci_dev, PvrdmaRing **ring,
                           uint64_t pdir_dma, uint32_t max_wr,
                           uint32_t max_sge, uint32_t nchunks)
{
    uint64_t *dir = NULL, *tbl = NULL;
    PvrdmaRing *r;
    int rc = -EINVAL;
    char ring_name[MAX_RING_NAME_SZ];
    uint32_t wqe_sz;

    if (!nchunks || nchunks > PVRDMA_MAX_FAST_REG_PAGES) {
        rdma_error_report("Got invalid page count for SRQ ring: %d",
                          nchunks);
        return rc;
    }

    dir = rdma_pci_dma_map(pci_dev, pdir_dma, TARGET_PAGE_SIZE);
    if (!dir) {
        rdma_error_report("Failed to map to SRQ page directory");
        goto out;
    }

    tbl = rdma_pci_dma_map(pci_dev, dir[0], TARGET_PAGE_SIZE);
    if (!tbl) {
        rdma_error_report("Failed to map to SRQ page table");
        goto out;
    }

    r = g_malloc(sizeof(*r));
    *ring = r;

    r->ring_state = (PvrdmaRingState *)
            rdma_pci_dma_map(pci_dev, tbl[0], TARGET_PAGE_SIZE);
    if (!r->ring_state) {
        rdma_error_report("Failed to map tp SRQ ring state");
        goto out_free_ring_mem;
    }

    wqe_sz = pow2ceil(sizeof(struct pvrdma_rq_wqe_hdr) +
                      sizeof(struct pvrdma_sge) * max_sge - 1);
    sprintf(ring_name, "srq_ring_%" PRIx64, pdir_dma);
    rc = pvrdma_ring_init(r, ring_name, pci_dev, &r->ring_state[1], max_wr,
                          wqe_sz, (dma_addr_t *)&tbl[1], nchunks - 1);
    if (rc) {
        goto out_unmap_ring_state;
    }

    goto out;

out_unmap_ring_state:
    rdma_pci_dma_unmap(pci_dev, r->ring_state, TARGET_PAGE_SIZE);

out_free_ring_mem:
    g_free(r);

out:
    rdma_pci_dma_unmap(pci_dev, tbl, TARGET_PAGE_SIZE);
    rdma_pci_dma_unmap(pci_dev, dir, TARGET_PAGE_SIZE);

    return rc;
}