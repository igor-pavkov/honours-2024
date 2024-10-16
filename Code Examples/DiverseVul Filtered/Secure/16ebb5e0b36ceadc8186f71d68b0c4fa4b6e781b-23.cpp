static int tc_dump_tclass(struct sk_buff *skb, struct netlink_callback *cb)
{
	struct tcmsg *tcm = (struct tcmsg*)NLMSG_DATA(cb->nlh);
	struct net *net = sock_net(skb->sk);
	struct netdev_queue *dev_queue;
	struct net_device *dev;
	int t, s_t;

	if (net != &init_net)
		return 0;

	if (cb->nlh->nlmsg_len < NLMSG_LENGTH(sizeof(*tcm)))
		return 0;
	if ((dev = dev_get_by_index(&init_net, tcm->tcm_ifindex)) == NULL)
		return 0;

	s_t = cb->args[0];
	t = 0;

	dev_queue = netdev_get_tx_queue(dev, 0);
	if (tc_dump_tclass_root(dev_queue->qdisc_sleeping, skb, tcm, cb, &t, s_t) < 0)
		goto done;

	dev_queue = &dev->rx_queue;
	if (tc_dump_tclass_root(dev_queue->qdisc_sleeping, skb, tcm, cb, &t, s_t) < 0)
		goto done;

done:
	cb->args[0] = t;

	dev_put(dev);
	return skb->len;
}