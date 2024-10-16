void dev_deactivate(struct net_device *dev)
{
	struct Qdisc *qdisc;
	struct sk_buff *skb;
	int running;

	spin_lock_bh(&dev->queue_lock);
	qdisc = dev->qdisc;
	dev->qdisc = &noop_qdisc;

	qdisc_reset(qdisc);

	skb = dev->gso_skb;
	dev->gso_skb = NULL;
	spin_unlock_bh(&dev->queue_lock);

	kfree_skb(skb);

	dev_watchdog_down(dev);

	/* Wait for outstanding qdisc-less dev_queue_xmit calls. */
	synchronize_rcu();

	/* Wait for outstanding qdisc_run calls. */
	do {
		while (test_bit(__LINK_STATE_QDISC_RUNNING, &dev->state))
			yield();

		/*
		 * Double-check inside queue lock to ensure that all effects
		 * of the queue run are visible when we return.
		 */
		spin_lock_bh(&dev->queue_lock);
		running = test_bit(__LINK_STATE_QDISC_RUNNING, &dev->state);
		spin_unlock_bh(&dev->queue_lock);

		/*
		 * The running flag should never be set at this point because
		 * we've already set dev->qdisc to noop_qdisc *inside* the same
		 * pair of spin locks.  That is, if any qdisc_run starts after
		 * our initial test it should see the noop_qdisc and then
		 * clear the RUNNING bit before dropping the queue lock.  So
		 * if it is set here then we've found a bug.
		 */
	} while (WARN_ON_ONCE(running));
}