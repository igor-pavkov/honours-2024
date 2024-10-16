static void skfp_ctl_set_multicast_list(struct net_device *dev)
{
	struct s_smc *smc = netdev_priv(dev);
	skfddi_priv *bp = &smc->os;
	unsigned long Flags;

	spin_lock_irqsave(&bp->DriverLock, Flags);
	skfp_ctl_set_multicast_list_wo_lock(dev);
	spin_unlock_irqrestore(&bp->DriverLock, Flags);
	return;
}				// skfp_ctl_set_multicast_list