u32 ethtool_op_get_sg(struct net_device *dev)
{
	return (dev->features & NETIF_F_SG) != 0;
}