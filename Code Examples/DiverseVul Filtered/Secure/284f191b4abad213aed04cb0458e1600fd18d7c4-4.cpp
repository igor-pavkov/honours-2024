static int create_cq_ring(PCIDevice *pci_dev , PvrdmaRing **ring,
                          uint64_t pdir_dma, uint32_t nchunks, uint32_t cqe)
{
    uint64_t *dir = NULL, *tbl = NULL;
    PvrdmaRing *r;
    int rc = -EINVAL;
    char ring_name[MAX_RING_NAME_SZ];

    if (!nchunks || nchunks > PVRDMA_MAX_FAST_REG_PAGES) {
        rdma_error_report("Got invalid nchunks: %d", nchunks);
        return rc;
    }

    dir = rdma_pci_dma_map(pci_dev, pdir_dma, TARGET_PAGE_SIZE);
    if (!dir) {
        rdma_error_report("Failed to map to CQ page directory");
        goto out;
    }

    tbl = rdma_pci_dma_map(pci_dev, dir[0], TARGET_PAGE_SIZE);
    if (!tbl) {
        rdma_error_report("Failed to map to CQ page table");
        goto out;
    }

    r = g_malloc(sizeof(*r));
    *ring = r;

    r->ring_state = (PvrdmaRingState *)
        rdma_pci_dma_map(pci_dev, tbl[0], TARGET_PAGE_SIZE);

    if (!r->ring_state) {
        rdma_error_report("Failed to map to CQ ring state");
        goto out_free_ring;
    }

    sprintf(ring_name, "cq_ring_%" PRIx64, pdir_dma);
    rc = pvrdma_ring_init(r, ring_name, pci_dev, &r->ring_state[1],
                          cqe, sizeof(struct pvrdma_cqe),
                          /* first page is ring state */
                          (dma_addr_t *)&tbl[1], nchunks - 1);
    if (rc) {
        goto out_unmap_ring_state;
    }

    goto out;

out_unmap_ring_state:
    /* ring_state was in slot 1, not 0 so need to jump back */
    rdma_pci_dma_unmap(pci_dev, --r->ring_state, TARGET_PAGE_SIZE);

out_free_ring:
    g_free(r);

out:
    rdma_pci_dma_unmap(pci_dev, tbl, TARGET_PAGE_SIZE);
    rdma_pci_dma_unmap(pci_dev, dir, TARGET_PAGE_SIZE);

    return rc;
}