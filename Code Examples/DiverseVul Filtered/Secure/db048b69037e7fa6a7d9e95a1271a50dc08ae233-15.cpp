u32 ethtool_op_get_tx_csum(struct net_device *dev)
{
	return (dev->features & NETIF_F_ALL_CSUM) != 0;
}