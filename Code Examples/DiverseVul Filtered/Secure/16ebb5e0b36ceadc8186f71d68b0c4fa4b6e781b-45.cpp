static int qdisc_graft(struct net_device *dev, struct Qdisc *parent,
		       struct sk_buff *skb, struct nlmsghdr *n, u32 classid,
		       struct Qdisc *new, struct Qdisc *old)
{
	struct Qdisc *q = old;
	int err = 0;

	if (parent == NULL) {
		unsigned int i, num_q, ingress;

		ingress = 0;
		num_q = dev->num_tx_queues;
		if ((q && q->flags & TCQ_F_INGRESS) ||
		    (new && new->flags & TCQ_F_INGRESS)) {
			num_q = 1;
			ingress = 1;
		}

		if (dev->flags & IFF_UP)
			dev_deactivate(dev);

		for (i = 0; i < num_q; i++) {
			struct netdev_queue *dev_queue = &dev->rx_queue;

			if (!ingress)
				dev_queue = netdev_get_tx_queue(dev, i);

			old = dev_graft_qdisc(dev_queue, new);
			if (new && i > 0)
				atomic_inc(&new->refcnt);

			notify_and_destroy(skb, n, classid, old, new);
		}

		if (dev->flags & IFF_UP)
			dev_activate(dev);
	} else {
		const struct Qdisc_class_ops *cops = parent->ops->cl_ops;

		err = -EINVAL;

		if (cops) {
			unsigned long cl = cops->get(parent, classid);
			if (cl) {
				err = cops->graft(parent, cl, new, &old);
				cops->put(parent, cl);
			}
		}
		if (!err)
			notify_and_destroy(skb, n, classid, old, new);
	}
	return err;
}