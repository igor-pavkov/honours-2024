static void fm10k_type_trans(struct fm10k_ring *rx_ring,
			     union fm10k_rx_desc __maybe_unused *rx_desc,
			     struct sk_buff *skb)
{
	struct net_device *dev = rx_ring->netdev;
	struct fm10k_l2_accel *l2_accel = rcu_dereference_bh(rx_ring->l2_accel);

	/* check to see if DGLORT belongs to a MACVLAN */
	if (l2_accel) {
		u16 idx = le16_to_cpu(FM10K_CB(skb)->fi.w.dglort) - 1;

		idx -= l2_accel->dglort;
		if (idx < l2_accel->size && l2_accel->macvlan[idx])
			dev = l2_accel->macvlan[idx];
		else
			l2_accel = NULL;
	}

	/* Record Rx queue, or update macvlan statistics */
	if (!l2_accel)
		skb_record_rx_queue(skb, rx_ring->queue_index);
	else
		macvlan_count_rx(netdev_priv(dev), skb->len + ETH_HLEN, true,
				 false);

	skb->protocol = eth_type_trans(skb, dev);
}