static void ResetAdapter(struct s_smc *smc)
{

	PRINTK(KERN_INFO "[fddi: ResetAdapter]\n");

	// Stop the adapter.

	card_stop(smc);		// Stop all activity.

	// Clear the transmit and receive descriptor queues.
	mac_drv_clear_tx_queue(smc);
	mac_drv_clear_rx_queue(smc);

	// Restart the adapter.

	smt_reset_defaults(smc, 1);	// Initialize the SMT module.

	init_smt(smc, (smc->os.dev)->dev_addr);	// Initialize the hardware.

	smt_online(smc, 1);	// Insert into the ring again.
	STI_FBI();

	// Restore original receive mode (multicasts, promiscuous, etc.).
	skfp_ctl_set_multicast_list_wo_lock(smc->os.dev);
}				// ResetAdapter