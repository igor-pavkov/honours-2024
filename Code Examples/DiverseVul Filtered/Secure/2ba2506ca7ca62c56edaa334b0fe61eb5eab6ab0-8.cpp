static inline int qdisc_restart(struct net_device *dev)
{
	struct Qdisc *q = dev->qdisc;
	struct sk_buff *skb;
	int ret = NETDEV_TX_BUSY;

	/* Dequeue packet */
	if (unlikely((skb = dev_dequeue_skb(dev, q)) == NULL))
		return 0;


	/* And release queue */
	spin_unlock(&dev->queue_lock);

	HARD_TX_LOCK(dev, smp_processor_id());
	if (!netif_subqueue_stopped(dev, skb))
		ret = dev_hard_start_xmit(skb, dev);
	HARD_TX_UNLOCK(dev);

	spin_lock(&dev->queue_lock);
	q = dev->qdisc;

	switch (ret) {
	case NETDEV_TX_OK:
		/* Driver sent out skb successfully */
		ret = qdisc_qlen(q);
		break;

	case NETDEV_TX_LOCKED:
		/* Driver try lock failed */
		ret = handle_dev_cpu_collision(skb, dev, q);
		break;

	default:
		/* Driver returned NETDEV_TX_BUSY - requeue skb */
		if (unlikely (ret != NETDEV_TX_BUSY && net_ratelimit()))
			printk(KERN_WARNING "BUG %s code %d qlen %d\n",
			       dev->name, ret, q->q.qlen);

		ret = dev_requeue_skb(skb, dev, q);
		break;
	}

	return ret;
}