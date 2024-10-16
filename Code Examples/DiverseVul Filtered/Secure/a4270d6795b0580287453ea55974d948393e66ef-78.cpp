int register_netdevice_notifier(struct notifier_block *nb)
{
	struct net_device *dev;
	struct net_device *last;
	struct net *net;
	int err;

	/* Close race with setup_net() and cleanup_net() */
	down_write(&pernet_ops_rwsem);
	rtnl_lock();
	err = raw_notifier_chain_register(&netdev_chain, nb);
	if (err)
		goto unlock;
	if (dev_boot_phase)
		goto unlock;
	for_each_net(net) {
		for_each_netdev(net, dev) {
			err = call_netdevice_notifier(nb, NETDEV_REGISTER, dev);
			err = notifier_to_errno(err);
			if (err)
				goto rollback;

			if (!(dev->flags & IFF_UP))
				continue;

			call_netdevice_notifier(nb, NETDEV_UP, dev);
		}
	}

unlock:
	rtnl_unlock();
	up_write(&pernet_ops_rwsem);
	return err;

rollback:
	last = dev;
	for_each_net(net) {
		for_each_netdev(net, dev) {
			if (dev == last)
				goto outroll;

			if (dev->flags & IFF_UP) {
				call_netdevice_notifier(nb, NETDEV_GOING_DOWN,
							dev);
				call_netdevice_notifier(nb, NETDEV_DOWN, dev);
			}
			call_netdevice_notifier(nb, NETDEV_UNREGISTER, dev);
		}
	}

outroll:
	raw_notifier_chain_unregister(&netdev_chain, nb);
	goto unlock;
}