static void destroy_qp_rings(PvrdmaRing *ring, uint8_t is_srq)
{
    pvrdma_ring_free(&ring[0]);
    if (!is_srq) {
        pvrdma_ring_free(&ring[1]);
    }

    rdma_pci_dma_unmap(ring->dev, ring->ring_state, TARGET_PAGE_SIZE);
    g_free(ring);
}