u32 ethtool_op_get_ufo(struct net_device *dev)
{
	return (dev->features & NETIF_F_UFO) != 0;
}