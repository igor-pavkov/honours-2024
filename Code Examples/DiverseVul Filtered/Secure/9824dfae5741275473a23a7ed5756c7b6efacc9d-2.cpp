static struct net_device * __init ipddp_init(void)
{
	static unsigned version_printed;
	struct net_device *dev;
	int err;

	dev = alloc_etherdev(0);
	if (!dev)
		return ERR_PTR(-ENOMEM);

	netif_keep_dst(dev);
	strcpy(dev->name, "ipddp%d");

	if (version_printed++ == 0)
                printk(version);

	/* Initialize the device structure. */
	dev->netdev_ops = &ipddp_netdev_ops;

        dev->type = ARPHRD_IPDDP;       	/* IP over DDP tunnel */
        dev->mtu = 585;
        dev->flags |= IFF_NOARP;

        /*
         *      The worst case header we will need is currently a
         *      ethernet header (14 bytes) and a ddp header (sizeof ddpehdr+1)
         *      We send over SNAP so that takes another 8 bytes.
         */
        dev->hard_header_len = 14+8+sizeof(struct ddpehdr)+1;

	err = register_netdev(dev);
	if (err) {
		free_netdev(dev);
		return ERR_PTR(err);
	}

	/* Let the user now what mode we are in */
	if(ipddp_mode == IPDDP_ENCAP)
		printk("%s: Appletalk-IP Encap. mode by Bradford W. Johnson <johns393@maroon.tc.umn.edu>\n", 
			dev->name);
	if(ipddp_mode == IPDDP_DECAP)
		printk("%s: Appletalk-IP Decap. mode by Jay Schulist <jschlst@samba.org>\n", 
			dev->name);

        return dev;
}