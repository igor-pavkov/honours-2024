int ethtool_op_set_flags(struct net_device *dev, u32 data)
{
	const struct ethtool_ops *ops = dev->ethtool_ops;
	unsigned long features = dev->features;

	if (data & ETH_FLAG_LRO)
		features |= NETIF_F_LRO;
	else
		features &= ~NETIF_F_LRO;

	if (data & ETH_FLAG_NTUPLE) {
		if (!ops->set_rx_ntuple)
			return -EOPNOTSUPP;
		features |= NETIF_F_NTUPLE;
	} else {
		/* safe to clear regardless */
		features &= ~NETIF_F_NTUPLE;
	}

	if (data & ETH_FLAG_RXHASH)
		features |= NETIF_F_RXHASH;
	else
		features &= ~NETIF_F_RXHASH;

	dev->features = features;
	return 0;
}