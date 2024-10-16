static int create_qp_rings(PCIDevice *pci_dev, uint64_t pdir_dma,
                           PvrdmaRing **rings, uint32_t scqe, uint32_t smax_sge,
                           uint32_t spages, uint32_t rcqe, uint32_t rmax_sge,
                           uint32_t rpages, uint8_t is_srq)
{
    uint64_t *dir = NULL, *tbl = NULL;
    PvrdmaRing *sr, *rr;
    int rc = -EINVAL;
    char ring_name[MAX_RING_NAME_SZ];
    uint32_t wqe_sz;

    if (!spages || spages > PVRDMA_MAX_FAST_REG_PAGES) {
        rdma_error_report("Got invalid send page count for QP ring: %d",
                          spages);
        return rc;
    }

    if (!is_srq && (!rpages || rpages > PVRDMA_MAX_FAST_REG_PAGES)) {
        rdma_error_report("Got invalid recv page count for QP ring: %d",
                          rpages);
        return rc;
    }

    dir = rdma_pci_dma_map(pci_dev, pdir_dma, TARGET_PAGE_SIZE);
    if (!dir) {
        rdma_error_report("Failed to map to QP page directory");
        goto out;
    }

    tbl = rdma_pci_dma_map(pci_dev, dir[0], TARGET_PAGE_SIZE);
    if (!tbl) {
        rdma_error_report("Failed to map to QP page table");
        goto out;
    }

    if (!is_srq) {
        sr = g_malloc(2 * sizeof(*rr));
        rr = &sr[1];
    } else {
        sr = g_malloc(sizeof(*sr));
    }

    *rings = sr;

    /* Create send ring */
    sr->ring_state = (PvrdmaRingState *)
        rdma_pci_dma_map(pci_dev, tbl[0], TARGET_PAGE_SIZE);
    if (!sr->ring_state) {
        rdma_error_report("Failed to map to QP ring state");
        goto out_free_sr_mem;
    }

    wqe_sz = pow2ceil(sizeof(struct pvrdma_sq_wqe_hdr) +
                      sizeof(struct pvrdma_sge) * smax_sge - 1);

    sprintf(ring_name, "qp_sring_%" PRIx64, pdir_dma);
    rc = pvrdma_ring_init(sr, ring_name, pci_dev, sr->ring_state,
                          scqe, wqe_sz, (dma_addr_t *)&tbl[1], spages);
    if (rc) {
        goto out_unmap_ring_state;
    }

    if (!is_srq) {
        /* Create recv ring */
        rr->ring_state = &sr->ring_state[1];
        wqe_sz = pow2ceil(sizeof(struct pvrdma_rq_wqe_hdr) +
                          sizeof(struct pvrdma_sge) * rmax_sge - 1);
        sprintf(ring_name, "qp_rring_%" PRIx64, pdir_dma);
        rc = pvrdma_ring_init(rr, ring_name, pci_dev, rr->ring_state,
                              rcqe, wqe_sz, (dma_addr_t *)&tbl[1 + spages],
                              rpages);
        if (rc) {
            goto out_free_sr;
        }
    }

    goto out;

out_free_sr:
    pvrdma_ring_free(sr);

out_unmap_ring_state:
    rdma_pci_dma_unmap(pci_dev, sr->ring_state, TARGET_PAGE_SIZE);

out_free_sr_mem:
    g_free(sr);

out:
    rdma_pci_dma_unmap(pci_dev, tbl, TARGET_PAGE_SIZE);
    rdma_pci_dma_unmap(pci_dev, dir, TARGET_PAGE_SIZE);

    return rc;
}