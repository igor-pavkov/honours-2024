static struct net_device *packet_cached_dev_get(struct packet_sock *po)
{
	struct net_device *dev;

	rcu_read_lock();
	dev = rcu_dereference(po->cached_dev);
	dev_hold(dev);
	rcu_read_unlock();

	return dev;
}