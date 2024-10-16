void dev_activate(struct net_device *dev)
{
	/* No queueing discipline is attached to device;
	   create default one i.e. pfifo_fast for devices,
	   which need queueing and noqueue_qdisc for
	   virtual interfaces
	 */

	if (dev->qdisc_sleeping == &noop_qdisc) {
		struct Qdisc *qdisc;
		if (dev->tx_queue_len) {
			qdisc = qdisc_create_dflt(dev, &pfifo_fast_ops,
						  TC_H_ROOT);
			if (qdisc == NULL) {
				printk(KERN_INFO "%s: activation failed\n", dev->name);
				return;
			}
			list_add_tail(&qdisc->list, &dev->qdisc_list);
		} else {
			qdisc =  &noqueue_qdisc;
		}
		dev->qdisc_sleeping = qdisc;
	}

	if (!netif_carrier_ok(dev))
		/* Delay activation until next carrier-on event */
		return;

	spin_lock_bh(&dev->queue_lock);
	rcu_assign_pointer(dev->qdisc, dev->qdisc_sleeping);
	if (dev->qdisc != &noqueue_qdisc) {
		dev->trans_start = jiffies;
		dev_watchdog_up(dev);
	}
	spin_unlock_bh(&dev->queue_lock);
}