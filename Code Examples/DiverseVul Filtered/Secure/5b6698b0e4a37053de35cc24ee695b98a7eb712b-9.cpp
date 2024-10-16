bool batadv_frag_send_packet(struct sk_buff *skb,
			     struct batadv_orig_node *orig_node,
			     struct batadv_neigh_node *neigh_node)
{
	struct batadv_priv *bat_priv;
	struct batadv_hard_iface *primary_if = NULL;
	struct batadv_frag_packet frag_header;
	struct sk_buff *skb_fragment;
	unsigned mtu = neigh_node->if_incoming->net_dev->mtu;
	unsigned header_size = sizeof(frag_header);
	unsigned max_fragment_size, max_packet_size;
	bool ret = false;

	/* To avoid merge and refragmentation at next-hops we never send
	 * fragments larger than BATADV_FRAG_MAX_FRAG_SIZE
	 */
	mtu = min_t(unsigned, mtu, BATADV_FRAG_MAX_FRAG_SIZE);
	max_fragment_size = (mtu - header_size - ETH_HLEN);
	max_packet_size = max_fragment_size * BATADV_FRAG_MAX_FRAGMENTS;

	/* Don't even try to fragment, if we need more than 16 fragments */
	if (skb->len > max_packet_size)
		goto out_err;

	bat_priv = orig_node->bat_priv;
	primary_if = batadv_primary_if_get_selected(bat_priv);
	if (!primary_if)
		goto out_err;

	/* Create one header to be copied to all fragments */
	frag_header.packet_type = BATADV_UNICAST_FRAG;
	frag_header.version = BATADV_COMPAT_VERSION;
	frag_header.ttl = BATADV_TTL;
	frag_header.seqno = htons(atomic_inc_return(&bat_priv->frag_seqno));
	frag_header.reserved = 0;
	frag_header.no = 0;
	frag_header.total_size = htons(skb->len);
	ether_addr_copy(frag_header.orig, primary_if->net_dev->dev_addr);
	ether_addr_copy(frag_header.dest, orig_node->orig);

	/* Eat and send fragments from the tail of skb */
	while (skb->len > max_fragment_size) {
		skb_fragment = batadv_frag_create(skb, &frag_header, mtu);
		if (!skb_fragment)
			goto out_err;

		batadv_inc_counter(bat_priv, BATADV_CNT_FRAG_TX);
		batadv_add_counter(bat_priv, BATADV_CNT_FRAG_TX_BYTES,
				   skb_fragment->len + ETH_HLEN);
		batadv_send_skb_packet(skb_fragment, neigh_node->if_incoming,
				       neigh_node->addr);
		frag_header.no++;

		/* The initial check in this function should cover this case */
		if (frag_header.no == BATADV_FRAG_MAX_FRAGMENTS - 1)
			goto out_err;
	}

	/* Make room for the fragment header. */
	if (batadv_skb_head_push(skb, header_size) < 0 ||
	    pskb_expand_head(skb, header_size + ETH_HLEN, 0, GFP_ATOMIC) < 0)
		goto out_err;

	memcpy(skb->data, &frag_header, header_size);

	/* Send the last fragment */
	batadv_inc_counter(bat_priv, BATADV_CNT_FRAG_TX);
	batadv_add_counter(bat_priv, BATADV_CNT_FRAG_TX_BYTES,
			   skb->len + ETH_HLEN);
	batadv_send_skb_packet(skb, neigh_node->if_incoming, neigh_node->addr);

	ret = true;

out_err:
	if (primary_if)
		batadv_hardif_free_ref(primary_if);

	return ret;
}