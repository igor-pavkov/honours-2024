static int skfp_close(struct net_device *dev)
{
	struct s_smc *smc = netdev_priv(dev);
	skfddi_priv *bp = &smc->os;

	CLI_FBI();
	smt_reset_defaults(smc, 1);
	card_stop(smc);
	mac_drv_clear_tx_queue(smc);
	mac_drv_clear_rx_queue(smc);

	netif_stop_queue(dev);
	/* Deregister (free) IRQ */
	free_irq(dev->irq, dev);

	skb_queue_purge(&bp->SendSkbQueue);
	bp->QueueSkb = MAX_TX_QUEUE_LEN;

	return (0);
}				// skfp_close