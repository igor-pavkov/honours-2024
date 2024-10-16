static int yam_set_mac_address(struct net_device *dev, void *addr)
{
	struct sockaddr *sa = (struct sockaddr *) addr;

	/* addr is an AX.25 shifted ASCII mac address */
	memcpy(dev->dev_addr, sa->sa_data, dev->addr_len);
	return 0;
}