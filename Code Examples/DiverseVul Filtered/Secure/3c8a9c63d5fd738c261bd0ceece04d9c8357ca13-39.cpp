static u32 tun_get_link(struct net_device *dev)
{
	struct tun_struct *tun = netdev_priv(dev);
	return !!tun->tfile;
}