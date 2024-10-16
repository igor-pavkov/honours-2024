void dev_init_scheduler(struct net_device *dev)
{
	qdisc_lock_tree(dev);
	dev->qdisc = &noop_qdisc;
	dev->qdisc_sleeping = &noop_qdisc;
	INIT_LIST_HEAD(&dev->qdisc_list);
	qdisc_unlock_tree(dev);

	setup_timer(&dev->watchdog_timer, dev_watchdog, (unsigned long)dev);
}