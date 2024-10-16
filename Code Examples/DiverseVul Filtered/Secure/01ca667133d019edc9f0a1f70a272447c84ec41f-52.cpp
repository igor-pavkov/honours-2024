static void fm10k_reuse_rx_page(struct fm10k_ring *rx_ring,
				struct fm10k_rx_buffer *old_buff)
{
	struct fm10k_rx_buffer *new_buff;
	u16 nta = rx_ring->next_to_alloc;

	new_buff = &rx_ring->rx_buffer[nta];

	/* update, and store next to alloc */
	nta++;
	rx_ring->next_to_alloc = (nta < rx_ring->count) ? nta : 0;

	/* transfer page from old buffer to new buffer */
	*new_buff = *old_buff;

	/* sync the buffer for use by the device */
	dma_sync_single_range_for_device(rx_ring->dev, old_buff->dma,
					 old_buff->page_offset,
					 FM10K_RX_BUFSZ,
					 DMA_FROM_DEVICE);
}