static int tun_set_rx_csum(struct net_device *dev, u32 data)
{
	struct tun_struct *tun = netdev_priv(dev);
	if (data)
		tun->flags &= ~TUN_NOCHECKSUM;
	else
		tun->flags |= TUN_NOCHECKSUM;
	return 0;
}