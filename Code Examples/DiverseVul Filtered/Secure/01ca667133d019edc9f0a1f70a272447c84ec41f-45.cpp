static void fm10k_tx_csum(struct fm10k_ring *tx_ring,
			  struct fm10k_tx_buffer *first)
{
	struct sk_buff *skb = first->skb;
	struct fm10k_tx_desc *tx_desc;
	union {
		struct iphdr *ipv4;
		struct ipv6hdr *ipv6;
		u8 *raw;
	} network_hdr;
	u8 *transport_hdr;
	__be16 frag_off;
	__be16 protocol;
	u8 l4_hdr = 0;

	if (skb->ip_summed != CHECKSUM_PARTIAL)
		goto no_csum;

	if (skb->encapsulation) {
		protocol = fm10k_tx_encap_offload(skb);
		if (!protocol) {
			if (skb_checksum_help(skb)) {
				dev_warn(tx_ring->dev,
					 "failed to offload encap csum!\n");
				tx_ring->tx_stats.csum_err++;
			}
			goto no_csum;
		}
		network_hdr.raw = skb_inner_network_header(skb);
		transport_hdr = skb_inner_transport_header(skb);
	} else {
		protocol = vlan_get_protocol(skb);
		network_hdr.raw = skb_network_header(skb);
		transport_hdr = skb_transport_header(skb);
	}

	switch (protocol) {
	case htons(ETH_P_IP):
		l4_hdr = network_hdr.ipv4->protocol;
		break;
	case htons(ETH_P_IPV6):
		l4_hdr = network_hdr.ipv6->nexthdr;
		if (likely((transport_hdr - network_hdr.raw) ==
			   sizeof(struct ipv6hdr)))
			break;
		ipv6_skip_exthdr(skb, network_hdr.raw - skb->data +
				      sizeof(struct ipv6hdr),
				 &l4_hdr, &frag_off);
		if (unlikely(frag_off))
			l4_hdr = NEXTHDR_FRAGMENT;
		break;
	default:
		break;
	}

	switch (l4_hdr) {
	case IPPROTO_TCP:
	case IPPROTO_UDP:
		break;
	case IPPROTO_GRE:
		if (skb->encapsulation)
			break;
		/* fall through */
	default:
		if (unlikely(net_ratelimit())) {
			dev_warn(tx_ring->dev,
				 "partial checksum, version=%d l4 proto=%x\n",
				 protocol, l4_hdr);
		}
		skb_checksum_help(skb);
		tx_ring->tx_stats.csum_err++;
		goto no_csum;
	}

	/* update TX checksum flag */
	first->tx_flags |= FM10K_TX_FLAGS_CSUM;
	tx_ring->tx_stats.csum_good++;

no_csum:
	/* populate Tx descriptor header size and mss */
	tx_desc = FM10K_TX_DESC(tx_ring, tx_ring->next_to_use);
	tx_desc->hdrlen = 0;
	tx_desc->mss = 0;
}