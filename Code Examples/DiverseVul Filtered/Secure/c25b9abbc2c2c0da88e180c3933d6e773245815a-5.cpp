static int skfp_open(struct net_device *dev)
{
	struct s_smc *smc = netdev_priv(dev);
	int err;

	PRINTK(KERN_INFO "entering skfp_open\n");
	/* Register IRQ - support shared interrupts by passing device ptr */
	err = request_irq(dev->irq, skfp_interrupt, IRQF_SHARED,
			  dev->name, dev);
	if (err)
		return err;

	/*
	 * Set current address to factory MAC address
	 *
	 * Note: We've already done this step in skfp_driver_init.
	 *       However, it's possible that a user has set a node
	 *               address override, then closed and reopened the
	 *               adapter.  Unless we reset the device address field
	 *               now, we'll continue to use the existing modified
	 *               address.
	 */
	read_address(smc, NULL);
	memcpy(dev->dev_addr, smc->hw.fddi_canon_addr.a, 6);

	init_smt(smc, NULL);
	smt_online(smc, 1);
	STI_FBI();

	/* Clear local multicast address tables */
	mac_clear_multicast(smc);

	/* Disable promiscuous filter settings */
	mac_drv_rx_mode(smc, RX_DISABLE_PROMISC);

	netif_start_queue(dev);
	return (0);
}				// skfp_open