struct Qdisc *qdisc_lookup(struct net_device *dev, u32 handle)
{
	unsigned int i;
	struct Qdisc *q;

	for (i = 0; i < dev->num_tx_queues; i++) {
		struct netdev_queue *txq = netdev_get_tx_queue(dev, i);
		struct Qdisc *txq_root = txq->qdisc_sleeping;

		q = qdisc_match_from_root(txq_root, handle);
		if (q)
			goto out;
	}

	q = qdisc_match_from_root(dev->rx_queue.qdisc_sleeping, handle);
out:
	return q;
}