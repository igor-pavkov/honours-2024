static int fm10k_clean_rx_irq(struct fm10k_q_vector *q_vector,
			      struct fm10k_ring *rx_ring,
			      int budget)
{
	struct sk_buff *skb = rx_ring->skb;
	unsigned int total_bytes = 0, total_packets = 0;
	u16 cleaned_count = fm10k_desc_unused(rx_ring);

	while (likely(total_packets < budget)) {
		union fm10k_rx_desc *rx_desc;

		/* return some buffers to hardware, one at a time is too slow */
		if (cleaned_count >= FM10K_RX_BUFFER_WRITE) {
			fm10k_alloc_rx_buffers(rx_ring, cleaned_count);
			cleaned_count = 0;
		}

		rx_desc = FM10K_RX_DESC(rx_ring, rx_ring->next_to_clean);

		if (!rx_desc->d.staterr)
			break;

		/* This memory barrier is needed to keep us from reading
		 * any other fields out of the rx_desc until we know the
		 * descriptor has been written back
		 */
		dma_rmb();

		/* retrieve a buffer from the ring */
		skb = fm10k_fetch_rx_buffer(rx_ring, rx_desc, skb);

		/* exit if we failed to retrieve a buffer */
		if (!skb)
			break;

		cleaned_count++;

		/* fetch next buffer in frame if non-eop */
		if (fm10k_is_non_eop(rx_ring, rx_desc))
			continue;

		/* verify the packet layout is correct */
		if (fm10k_cleanup_headers(rx_ring, rx_desc, skb)) {
			skb = NULL;
			continue;
		}

		/* populate checksum, timestamp, VLAN, and protocol */
		total_bytes += fm10k_process_skb_fields(rx_ring, rx_desc, skb);

		fm10k_receive_skb(q_vector, skb);

		/* reset skb pointer */
		skb = NULL;

		/* update budget accounting */
		total_packets++;
	}

	/* place incomplete frames back on ring for completion */
	rx_ring->skb = skb;

	u64_stats_update_begin(&rx_ring->syncp);
	rx_ring->stats.packets += total_packets;
	rx_ring->stats.bytes += total_bytes;
	u64_stats_update_end(&rx_ring->syncp);
	q_vector->rx.total_packets += total_packets;
	q_vector->rx.total_bytes += total_bytes;

	return total_packets;
}