static int tun_flags(struct tun_struct *tun)
{
	int flags = 0;

	if (tun->flags & TUN_TUN_DEV)
		flags |= IFF_TUN;
	else
		flags |= IFF_TAP;

	if (tun->flags & TUN_NO_PI)
		flags |= IFF_NO_PI;

	if (tun->flags & TUN_ONE_QUEUE)
		flags |= IFF_ONE_QUEUE;

	if (tun->flags & TUN_VNET_HDR)
		flags |= IFF_VNET_HDR;

	return flags;
}