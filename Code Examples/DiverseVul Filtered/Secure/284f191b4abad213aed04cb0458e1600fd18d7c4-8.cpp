static void destroy_srq_ring(PvrdmaRing *ring)
{
    pvrdma_ring_free(ring);
    rdma_pci_dma_unmap(ring->dev, ring->ring_state, TARGET_PAGE_SIZE);
    g_free(ring);
}