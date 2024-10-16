int unregister_netdevice_notifier(struct notifier_block *nb)
{
	struct net_device *dev;
	struct net *net;
	int err;

	/* Close race with setup_net() and cleanup_net() */
	down_write(&pernet_ops_rwsem);
	rtnl_lock();
	err = raw_notifier_chain_unregister(&netdev_chain, nb);
	if (err)
		goto unlock;

	for_each_net(net) {
		for_each_netdev(net, dev) {
			if (dev->flags & IFF_UP) {
				call_netdevice_notifier(nb, NETDEV_GOING_DOWN,
							dev);
				call_netdevice_notifier(nb, NETDEV_DOWN, dev);
			}
			call_netdevice_notifier(nb, NETDEV_UNREGISTER, dev);
		}
	}
unlock:
	rtnl_unlock();
	up_write(&pernet_ops_rwsem);
	return err;
}