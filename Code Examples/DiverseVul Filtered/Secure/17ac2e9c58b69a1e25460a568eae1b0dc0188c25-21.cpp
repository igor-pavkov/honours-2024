static void rose_set_lockdep_key(struct net_device *dev)
{
	lockdep_set_class(&dev->addr_list_lock, &rose_netdev_addr_lock_key);
	netdev_for_each_tx_queue(dev, rose_set_lockdep_one, NULL);
}