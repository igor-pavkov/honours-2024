static u32 tun_get_rx_csum(struct net_device *dev)
{
	struct tun_struct *tun = netdev_priv(dev);
	return (tun->flags & TUN_NOCHECKSUM) == 0;
}