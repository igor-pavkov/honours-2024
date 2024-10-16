static irqreturn_t skfp_interrupt(int irq, void *dev_id)
{
	struct net_device *dev = dev_id;
	struct s_smc *smc;	/* private board structure pointer */
	skfddi_priv *bp;

	smc = netdev_priv(dev);
	bp = &smc->os;

	// IRQs enabled or disabled ?
	if (inpd(ADDR(B0_IMSK)) == 0) {
		// IRQs are disabled: must be shared interrupt
		return IRQ_NONE;
	}
	// Note: At this point, IRQs are enabled.
	if ((inpd(ISR_A) & smc->hw.is_imask) == 0) {	// IRQ?
		// Adapter did not issue an IRQ: must be shared interrupt
		return IRQ_NONE;
	}
	CLI_FBI();		// Disable IRQs from our adapter.
	spin_lock(&bp->DriverLock);

	// Call interrupt handler in hardware module (HWM).
	fddi_isr(smc);

	if (smc->os.ResetRequested) {
		ResetAdapter(smc);
		smc->os.ResetRequested = FALSE;
	}
	spin_unlock(&bp->DriverLock);
	STI_FBI();		// Enable IRQs from our adapter.

	return IRQ_HANDLED;
}				// skfp_interrupt