static void destroy_cq_ring(PvrdmaRing *ring)
{
    pvrdma_ring_free(ring);
    /* ring_state was in slot 1, not 0 so need to jump back */
    rdma_pci_dma_unmap(ring->dev, --ring->ring_state, TARGET_PAGE_SIZE);
    g_free(ring);
}