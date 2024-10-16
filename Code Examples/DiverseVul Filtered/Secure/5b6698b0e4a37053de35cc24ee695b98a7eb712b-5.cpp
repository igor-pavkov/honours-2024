bool batadv_frag_skb_fwd(struct sk_buff *skb,
			 struct batadv_hard_iface *recv_if,
			 struct batadv_orig_node *orig_node_src)
{
	struct batadv_priv *bat_priv = netdev_priv(recv_if->soft_iface);
	struct batadv_orig_node *orig_node_dst = NULL;
	struct batadv_neigh_node *neigh_node = NULL;
	struct batadv_frag_packet *packet;
	uint16_t total_size;
	bool ret = false;

	packet = (struct batadv_frag_packet *)skb->data;
	orig_node_dst = batadv_orig_hash_find(bat_priv, packet->dest);
	if (!orig_node_dst)
		goto out;

	neigh_node = batadv_find_router(bat_priv, orig_node_dst, recv_if);
	if (!neigh_node)
		goto out;

	/* Forward the fragment, if the merged packet would be too big to
	 * be assembled.
	 */
	total_size = ntohs(packet->total_size);
	if (total_size > neigh_node->if_incoming->net_dev->mtu) {
		batadv_inc_counter(bat_priv, BATADV_CNT_FRAG_FWD);
		batadv_add_counter(bat_priv, BATADV_CNT_FRAG_FWD_BYTES,
				   skb->len + ETH_HLEN);

		packet->ttl--;
		batadv_send_skb_packet(skb, neigh_node->if_incoming,
				       neigh_node->addr);
		ret = true;
	}

out:
	if (orig_node_dst)
		batadv_orig_node_free_ref(orig_node_dst);
	if (neigh_node)
		batadv_neigh_node_free_ref(neigh_node);
	return ret;
}