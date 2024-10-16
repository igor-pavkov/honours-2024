static void fm10k_tx_map(struct fm10k_ring *tx_ring,
			 struct fm10k_tx_buffer *first)
{
	struct sk_buff *skb = first->skb;
	struct fm10k_tx_buffer *tx_buffer;
	struct fm10k_tx_desc *tx_desc;
	struct skb_frag_struct *frag;
	unsigned char *data;
	dma_addr_t dma;
	unsigned int data_len, size;
	u32 tx_flags = first->tx_flags;
	u16 i = tx_ring->next_to_use;
	u8 flags = fm10k_tx_desc_flags(skb, tx_flags);

	tx_desc = FM10K_TX_DESC(tx_ring, i);

	/* add HW VLAN tag */
	if (skb_vlan_tag_present(skb))
		tx_desc->vlan = cpu_to_le16(skb_vlan_tag_get(skb));
	else
		tx_desc->vlan = 0;

	size = skb_headlen(skb);
	data = skb->data;

	dma = dma_map_single(tx_ring->dev, data, size, DMA_TO_DEVICE);

	data_len = skb->data_len;
	tx_buffer = first;

	for (frag = &skb_shinfo(skb)->frags[0];; frag++) {
		if (dma_mapping_error(tx_ring->dev, dma))
			goto dma_error;

		/* record length, and DMA address */
		dma_unmap_len_set(tx_buffer, len, size);
		dma_unmap_addr_set(tx_buffer, dma, dma);

		while (unlikely(size > FM10K_MAX_DATA_PER_TXD)) {
			if (fm10k_tx_desc_push(tx_ring, tx_desc++, i++, dma,
					       FM10K_MAX_DATA_PER_TXD, flags)) {
				tx_desc = FM10K_TX_DESC(tx_ring, 0);
				i = 0;
			}

			dma += FM10K_MAX_DATA_PER_TXD;
			size -= FM10K_MAX_DATA_PER_TXD;
		}

		if (likely(!data_len))
			break;

		if (fm10k_tx_desc_push(tx_ring, tx_desc++, i++,
				       dma, size, flags)) {
			tx_desc = FM10K_TX_DESC(tx_ring, 0);
			i = 0;
		}

		size = skb_frag_size(frag);
		data_len -= size;

		dma = skb_frag_dma_map(tx_ring->dev, frag, 0, size,
				       DMA_TO_DEVICE);

		tx_buffer = &tx_ring->tx_buffer[i];
	}

	/* write last descriptor with LAST bit set */
	flags |= FM10K_TXD_FLAG_LAST;

	if (fm10k_tx_desc_push(tx_ring, tx_desc, i++, dma, size, flags))
		i = 0;

	/* record bytecount for BQL */
	netdev_tx_sent_queue(txring_txq(tx_ring), first->bytecount);

	/* record SW timestamp if HW timestamp is not available */
	skb_tx_timestamp(first->skb);

	/* Force memory writes to complete before letting h/w know there
	 * are new descriptors to fetch.  (Only applicable for weak-ordered
	 * memory model archs, such as IA-64).
	 *
	 * We also need this memory barrier to make certain all of the
	 * status bits have been updated before next_to_watch is written.
	 */
	wmb();

	/* set next_to_watch value indicating a packet is present */
	first->next_to_watch = tx_desc;

	tx_ring->next_to_use = i;

	/* Make sure there is space in the ring for the next send. */
	fm10k_maybe_stop_tx(tx_ring, DESC_NEEDED);

	/* notify HW of packet */
	if (netif_xmit_stopped(txring_txq(tx_ring)) || !skb->xmit_more) {
		writel(i, tx_ring->tail);

		/* we need this if more than one processor can write to our tail
		 * at a time, it synchronizes IO on IA64/Altix systems
		 */
		mmiowb();
	}

	return;
dma_error:
	dev_err(tx_ring->dev, "TX DMA map failed\n");

	/* clear dma mappings for failed tx_buffer map */
	for (;;) {
		tx_buffer = &tx_ring->tx_buffer[i];
		fm10k_unmap_and_free_tx_resource(tx_ring, tx_buffer);
		if (tx_buffer == first)
			break;
		if (i == 0)
			i = tx_ring->count;
		i--;
	}

	tx_ring->next_to_use = i;
}