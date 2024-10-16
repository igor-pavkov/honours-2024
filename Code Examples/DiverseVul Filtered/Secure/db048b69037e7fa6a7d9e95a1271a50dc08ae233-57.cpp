static noinline_for_stack int ethtool_set_rx_ntuple(struct net_device *dev,
						    void __user *useraddr)
{
	struct ethtool_rx_ntuple cmd;
	const struct ethtool_ops *ops = dev->ethtool_ops;
	struct ethtool_rx_ntuple_flow_spec_container *fsc = NULL;
	int ret;

	if (!(dev->features & NETIF_F_NTUPLE))
		return -EINVAL;

	if (copy_from_user(&cmd, useraddr, sizeof(cmd)))
		return -EFAULT;

	/*
	 * Cache filter in dev struct for GET operation only if
	 * the underlying driver doesn't have its own GET operation, and
	 * only if the filter was added successfully.  First make sure we
	 * can allocate the filter, then continue if successful.
	 */
	if (!ops->get_rx_ntuple) {
		fsc = kmalloc(sizeof(*fsc), GFP_ATOMIC);
		if (!fsc)
			return -ENOMEM;
	}

	ret = ops->set_rx_ntuple(dev, &cmd);
	if (ret) {
		kfree(fsc);
		return ret;
	}

	if (!ops->get_rx_ntuple)
		__rx_ntuple_filter_add(&dev->ethtool_ntuple_list, &cmd.fs, fsc);

	return ret;
}