static bool fm10k_clean_tx_irq(struct fm10k_q_vector *q_vector,
			       struct fm10k_ring *tx_ring, int napi_budget)
{
	struct fm10k_intfc *interface = q_vector->interface;
	struct fm10k_tx_buffer *tx_buffer;
	struct fm10k_tx_desc *tx_desc;
	unsigned int total_bytes = 0, total_packets = 0;
	unsigned int budget = q_vector->tx.work_limit;
	unsigned int i = tx_ring->next_to_clean;

	if (test_bit(__FM10K_DOWN, interface->state))
		return true;

	tx_buffer = &tx_ring->tx_buffer[i];
	tx_desc = FM10K_TX_DESC(tx_ring, i);
	i -= tx_ring->count;

	do {
		struct fm10k_tx_desc *eop_desc = tx_buffer->next_to_watch;

		/* if next_to_watch is not set then there is no work pending */
		if (!eop_desc)
			break;

		/* prevent any other reads prior to eop_desc */
		smp_rmb();

		/* if DD is not set pending work has not been completed */
		if (!(eop_desc->flags & FM10K_TXD_FLAG_DONE))
			break;

		/* clear next_to_watch to prevent false hangs */
		tx_buffer->next_to_watch = NULL;

		/* update the statistics for this packet */
		total_bytes += tx_buffer->bytecount;
		total_packets += tx_buffer->gso_segs;

		/* free the skb */
		napi_consume_skb(tx_buffer->skb, napi_budget);

		/* unmap skb header data */
		dma_unmap_single(tx_ring->dev,
				 dma_unmap_addr(tx_buffer, dma),
				 dma_unmap_len(tx_buffer, len),
				 DMA_TO_DEVICE);

		/* clear tx_buffer data */
		tx_buffer->skb = NULL;
		dma_unmap_len_set(tx_buffer, len, 0);

		/* unmap remaining buffers */
		while (tx_desc != eop_desc) {
			tx_buffer++;
			tx_desc++;
			i++;
			if (unlikely(!i)) {
				i -= tx_ring->count;
				tx_buffer = tx_ring->tx_buffer;
				tx_desc = FM10K_TX_DESC(tx_ring, 0);
			}

			/* unmap any remaining paged data */
			if (dma_unmap_len(tx_buffer, len)) {
				dma_unmap_page(tx_ring->dev,
					       dma_unmap_addr(tx_buffer, dma),
					       dma_unmap_len(tx_buffer, len),
					       DMA_TO_DEVICE);
				dma_unmap_len_set(tx_buffer, len, 0);
			}
		}

		/* move us one more past the eop_desc for start of next pkt */
		tx_buffer++;
		tx_desc++;
		i++;
		if (unlikely(!i)) {
			i -= tx_ring->count;
			tx_buffer = tx_ring->tx_buffer;
			tx_desc = FM10K_TX_DESC(tx_ring, 0);
		}

		/* issue prefetch for next Tx descriptor */
		prefetch(tx_desc);

		/* update budget accounting */
		budget--;
	} while (likely(budget));

	i += tx_ring->count;
	tx_ring->next_to_clean = i;
	u64_stats_update_begin(&tx_ring->syncp);
	tx_ring->stats.bytes += total_bytes;
	tx_ring->stats.packets += total_packets;
	u64_stats_update_end(&tx_ring->syncp);
	q_vector->tx.total_bytes += total_bytes;
	q_vector->tx.total_packets += total_packets;

	if (check_for_tx_hang(tx_ring) && fm10k_check_tx_hang(tx_ring)) {
		/* schedule immediate reset if we believe we hung */
		struct fm10k_hw *hw = &interface->hw;

		netif_err(interface, drv, tx_ring->netdev,
			  "Detected Tx Unit Hang\n"
			  "  Tx Queue             <%d>\n"
			  "  TDH, TDT             <%x>, <%x>\n"
			  "  next_to_use          <%x>\n"
			  "  next_to_clean        <%x>\n",
			  tx_ring->queue_index,
			  fm10k_read_reg(hw, FM10K_TDH(tx_ring->reg_idx)),
			  fm10k_read_reg(hw, FM10K_TDT(tx_ring->reg_idx)),
			  tx_ring->next_to_use, i);

		netif_stop_subqueue(tx_ring->netdev,
				    tx_ring->queue_index);

		netif_info(interface, probe, tx_ring->netdev,
			   "tx hang %d detected on queue %d, resetting interface\n",
			   interface->tx_timeout_count + 1,
			   tx_ring->queue_index);

		fm10k_tx_timeout_reset(interface);

		/* the netdev is about to reset, no point in enabling stuff */
		return true;
	}

	/* notify netdev of completed buffers */
	netdev_tx_completed_queue(txring_txq(tx_ring),
				  total_packets, total_bytes);

#define TX_WAKE_THRESHOLD min_t(u16, FM10K_MIN_TXD - 1, DESC_NEEDED * 2)
	if (unlikely(total_packets && netif_carrier_ok(tx_ring->netdev) &&
		     (fm10k_desc_unused(tx_ring) >= TX_WAKE_THRESHOLD))) {
		/* Make sure that anybody stopping the queue after this
		 * sees the new next_to_clean.
		 */
		smp_mb();
		if (__netif_subqueue_stopped(tx_ring->netdev,
					     tx_ring->queue_index) &&
		    !test_bit(__FM10K_DOWN, interface->state)) {
			netif_wake_subqueue(tx_ring->netdev,
					    tx_ring->queue_index);
			++tx_ring->tx_stats.restart_queue;
		}
	}

	return !!budget;
}