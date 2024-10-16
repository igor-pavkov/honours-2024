void llc_restart_tx(struct s_smc *smc)
{
	skfddi_priv *bp = &smc->os;

	PRINTK(KERN_INFO "[llc_restart_tx]\n");

	// Try to send queued packets
	spin_unlock(&bp->DriverLock);
	send_queued_packets(smc);
	spin_lock(&bp->DriverLock);
	netif_start_queue(bp->dev);// system may send again if it was blocked

}				// llc_restart_tx