netdev_tx_t fm10k_xmit_frame_ring(struct sk_buff *skb,
				  struct fm10k_ring *tx_ring)
{
	u16 count = TXD_USE_COUNT(skb_headlen(skb));
	struct fm10k_tx_buffer *first;
	unsigned short f;
	u32 tx_flags = 0;
	int tso;

	/* need: 1 descriptor per page * PAGE_SIZE/FM10K_MAX_DATA_PER_TXD,
	 *       + 1 desc for skb_headlen/FM10K_MAX_DATA_PER_TXD,
	 *       + 2 desc gap to keep tail from touching head
	 * otherwise try next time
	 */
	for (f = 0; f < skb_shinfo(skb)->nr_frags; f++)
		count += TXD_USE_COUNT(skb_shinfo(skb)->frags[f].size);

	if (fm10k_maybe_stop_tx(tx_ring, count + 3)) {
		tx_ring->tx_stats.tx_busy++;
		return NETDEV_TX_BUSY;
	}

	/* record the location of the first descriptor for this packet */
	first = &tx_ring->tx_buffer[tx_ring->next_to_use];
	first->skb = skb;
	first->bytecount = max_t(unsigned int, skb->len, ETH_ZLEN);
	first->gso_segs = 1;

	/* record initial flags and protocol */
	first->tx_flags = tx_flags;

	tso = fm10k_tso(tx_ring, first);
	if (tso < 0)
		goto out_drop;
	else if (!tso)
		fm10k_tx_csum(tx_ring, first);

	fm10k_tx_map(tx_ring, first);

	return NETDEV_TX_OK;

out_drop:
	dev_kfree_skb_any(first->skb);
	first->skb = NULL;

	return NETDEV_TX_OK;
}