static int tc_dump_qdisc(struct sk_buff *skb, struct netlink_callback *cb)
{
	struct net *net = sock_net(skb->sk);
	int idx, q_idx;
	int s_idx, s_q_idx;
	struct net_device *dev;

	if (net != &init_net)
		return 0;

	s_idx = cb->args[0];
	s_q_idx = q_idx = cb->args[1];
	read_lock(&dev_base_lock);
	idx = 0;
	for_each_netdev(&init_net, dev) {
		struct netdev_queue *dev_queue;

		if (idx < s_idx)
			goto cont;
		if (idx > s_idx)
			s_q_idx = 0;
		q_idx = 0;

		dev_queue = netdev_get_tx_queue(dev, 0);
		if (tc_dump_qdisc_root(dev_queue->qdisc_sleeping, skb, cb, &q_idx, s_q_idx) < 0)
			goto done;

		dev_queue = &dev->rx_queue;
		if (tc_dump_qdisc_root(dev_queue->qdisc_sleeping, skb, cb, &q_idx, s_q_idx) < 0)
			goto done;

cont:
		idx++;
	}

done:
	read_unlock(&dev_base_lock);

	cb->args[0] = idx;
	cb->args[1] = q_idx;

	return skb->len;
}