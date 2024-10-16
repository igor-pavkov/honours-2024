struct Qdisc * qdisc_create_dflt(struct net_device *dev, struct Qdisc_ops *ops,
				 unsigned int parentid)
{
	struct Qdisc *sch;

	sch = qdisc_alloc(dev, ops);
	if (IS_ERR(sch))
		goto errout;
	sch->stats_lock = &dev->queue_lock;
	sch->parent = parentid;

	if (!ops->init || ops->init(sch, NULL) == 0)
		return sch;

	qdisc_destroy(sch);
errout:
	return NULL;
}