static int skfp_ctl_set_mac_address(struct net_device *dev, void *addr)
{
	struct s_smc *smc = netdev_priv(dev);
	struct sockaddr *p_sockaddr = (struct sockaddr *) addr;
	skfddi_priv *bp = &smc->os;
	unsigned long Flags;


	memcpy(dev->dev_addr, p_sockaddr->sa_data, FDDI_K_ALEN);
	spin_lock_irqsave(&bp->DriverLock, Flags);
	ResetAdapter(smc);
	spin_unlock_irqrestore(&bp->DriverLock, Flags);

	return (0);		/* always return zero */
}				// skfp_ctl_set_mac_address