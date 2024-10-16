static bool fm10k_alloc_mapped_page(struct fm10k_ring *rx_ring,
				    struct fm10k_rx_buffer *bi)
{
	struct page *page = bi->page;
	dma_addr_t dma;

	/* Only page will be NULL if buffer was consumed */
	if (likely(page))
		return true;

	/* alloc new page for storage */
	page = dev_alloc_page();
	if (unlikely(!page)) {
		rx_ring->rx_stats.alloc_failed++;
		return false;
	}

	/* map page for use */
	dma = dma_map_page(rx_ring->dev, page, 0, PAGE_SIZE, DMA_FROM_DEVICE);

	/* if mapping failed free memory back to system since
	 * there isn't much point in holding memory we can't use
	 */
	if (dma_mapping_error(rx_ring->dev, dma)) {
		__free_page(page);

		rx_ring->rx_stats.alloc_failed++;
		return false;
	}

	bi->dma = dma;
	bi->page = page;
	bi->page_offset = 0;

	return true;
}