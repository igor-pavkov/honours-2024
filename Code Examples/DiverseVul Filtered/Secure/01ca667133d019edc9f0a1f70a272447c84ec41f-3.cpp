static int fm10k_tso(struct fm10k_ring *tx_ring,
		     struct fm10k_tx_buffer *first)
{
	struct sk_buff *skb = first->skb;
	struct fm10k_tx_desc *tx_desc;
	unsigned char *th;
	u8 hdrlen;

	if (skb->ip_summed != CHECKSUM_PARTIAL)
		return 0;

	if (!skb_is_gso(skb))
		return 0;

	/* compute header lengths */
	if (skb->encapsulation) {
		if (!fm10k_tx_encap_offload(skb))
			goto err_vxlan;
		th = skb_inner_transport_header(skb);
	} else {
		th = skb_transport_header(skb);
	}

	/* compute offset from SOF to transport header and add header len */
	hdrlen = (th - skb->data) + (((struct tcphdr *)th)->doff << 2);

	first->tx_flags |= FM10K_TX_FLAGS_CSUM;

	/* update gso size and bytecount with header size */
	first->gso_segs = skb_shinfo(skb)->gso_segs;
	first->bytecount += (first->gso_segs - 1) * hdrlen;

	/* populate Tx descriptor header size and mss */
	tx_desc = FM10K_TX_DESC(tx_ring, tx_ring->next_to_use);
	tx_desc->hdrlen = hdrlen;
	tx_desc->mss = cpu_to_le16(skb_shinfo(skb)->gso_size);

	return 1;

err_vxlan:
	tx_ring->netdev->features &= ~NETIF_F_GSO_UDP_TUNNEL;
	if (net_ratelimit())
		netdev_err(tx_ring->netdev,
			   "TSO requested for unsupported tunnel, disabling offload\n");
	return -1;
}