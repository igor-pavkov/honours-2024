int dev_change_name(struct net_device *dev, const char *newname)
{
	unsigned char old_assign_type;
	char oldname[IFNAMSIZ];
	int err = 0;
	int ret;
	struct net *net;

	ASSERT_RTNL();
	BUG_ON(!dev_net(dev));

	net = dev_net(dev);

	/* Some auto-enslaved devices e.g. failover slaves are
	 * special, as userspace might rename the device after
	 * the interface had been brought up and running since
	 * the point kernel initiated auto-enslavement. Allow
	 * live name change even when these slave devices are
	 * up and running.
	 *
	 * Typically, users of these auto-enslaving devices
	 * don't actually care about slave name change, as
	 * they are supposed to operate on master interface
	 * directly.
	 */
	if (dev->flags & IFF_UP &&
	    likely(!(dev->priv_flags & IFF_LIVE_RENAME_OK)))
		return -EBUSY;

	write_seqcount_begin(&devnet_rename_seq);

	if (strncmp(newname, dev->name, IFNAMSIZ) == 0) {
		write_seqcount_end(&devnet_rename_seq);
		return 0;
	}

	memcpy(oldname, dev->name, IFNAMSIZ);

	err = dev_get_valid_name(net, dev, newname);
	if (err < 0) {
		write_seqcount_end(&devnet_rename_seq);
		return err;
	}

	if (oldname[0] && !strchr(oldname, '%'))
		netdev_info(dev, "renamed from %s\n", oldname);

	old_assign_type = dev->name_assign_type;
	dev->name_assign_type = NET_NAME_RENAMED;

rollback:
	ret = device_rename(&dev->dev, dev->name);
	if (ret) {
		memcpy(dev->name, oldname, IFNAMSIZ);
		dev->name_assign_type = old_assign_type;
		write_seqcount_end(&devnet_rename_seq);
		return ret;
	}

	write_seqcount_end(&devnet_rename_seq);

	netdev_adjacent_rename_links(dev, oldname);

	write_lock_bh(&dev_base_lock);
	hlist_del_rcu(&dev->name_hlist);
	write_unlock_bh(&dev_base_lock);

	synchronize_rcu();

	write_lock_bh(&dev_base_lock);
	hlist_add_head_rcu(&dev->name_hlist, dev_name_hash(net, dev->name));
	write_unlock_bh(&dev_base_lock);

	ret = call_netdevice_notifiers(NETDEV_CHANGENAME, dev);
	ret = notifier_to_errno(ret);

	if (ret) {
		/* err >= 0 after dev_alloc_name() or stores the first errno */
		if (err >= 0) {
			err = ret;
			write_seqcount_begin(&devnet_rename_seq);
			memcpy(dev->name, oldname, IFNAMSIZ);
			memcpy(oldname, newname, IFNAMSIZ);
			dev->name_assign_type = old_assign_type;
			old_assign_type = NET_NAME_RENAMED;
			goto rollback;
		} else {
			pr_err("%s: name change rollback failed: %d\n",
			       dev->name, ret);
		}
	}

	return err;
}