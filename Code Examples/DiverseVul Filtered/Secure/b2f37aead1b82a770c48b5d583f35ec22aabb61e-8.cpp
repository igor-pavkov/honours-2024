static void ax_setup(struct net_device *dev)
{
	/* Finish setting up the DEVICE info. */
	dev->mtu             = AX_MTU;
	dev->hard_header_len = AX25_MAX_HEADER_LEN;
	dev->addr_len        = AX25_ADDR_LEN;
	dev->type            = ARPHRD_AX25;
	dev->tx_queue_len    = 10;
	dev->header_ops      = &ax25_header_ops;
	dev->netdev_ops	     = &ax_netdev_ops;


	memcpy(dev->broadcast, &ax25_bcast, AX25_ADDR_LEN);
	dev_addr_set(dev, (u8 *)&ax25_defaddr);

	dev->flags      = IFF_BROADCAST | IFF_MULTICAST;
}