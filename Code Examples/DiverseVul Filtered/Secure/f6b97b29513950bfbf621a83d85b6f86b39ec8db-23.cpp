static void nr_set_lockdep_key(struct net_device *dev)
{
	lockdep_set_class(&dev->addr_list_lock, &nr_netdev_addr_lock_key);
	netdev_for_each_tx_queue(dev, nr_set_lockdep_one, NULL);
}