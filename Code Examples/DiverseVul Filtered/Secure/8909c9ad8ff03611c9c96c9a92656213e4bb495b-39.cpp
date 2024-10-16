static struct netdev_queue *dev_pick_tx(struct net_device *dev,
					struct sk_buff *skb)
{
	int queue_index;
	const struct net_device_ops *ops = dev->netdev_ops;

	if (dev->real_num_tx_queues == 1)
		queue_index = 0;
	else if (ops->ndo_select_queue) {
		queue_index = ops->ndo_select_queue(dev, skb);
		queue_index = dev_cap_txqueue(dev, queue_index);
	} else {
		struct sock *sk = skb->sk;
		queue_index = sk_tx_queue_get(sk);

		if (queue_index < 0 || skb->ooo_okay ||
		    queue_index >= dev->real_num_tx_queues) {
			int old_index = queue_index;

			queue_index = get_xps_queue(dev, skb);
			if (queue_index < 0)
				queue_index = skb_tx_hash(dev, skb);

			if (queue_index != old_index && sk) {
				struct dst_entry *dst =
				    rcu_dereference_check(sk->sk_dst_cache, 1);

				if (dst && skb_dst(skb) == dst)
					sk_tx_queue_set(sk, queue_index);
			}
		}
	}

	skb_set_queue_mapping(skb, queue_index);
	return netdev_get_tx_queue(dev, queue_index);
}