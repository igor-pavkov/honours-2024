static void setup_struct_wldev_for_init(struct b43_wldev *dev)
{
	dev->dfq_valid = false;

	/* Assume the radio is enabled. If it's not enabled, the state will
	 * immediately get fixed on the first periodic work run. */
	dev->radio_hw_enable = true;

	/* Stats */
	memset(&dev->stats, 0, sizeof(dev->stats));

	setup_struct_phy_for_init(dev, &dev->phy);

	/* IRQ related flags */
	dev->irq_reason = 0;
	memset(dev->dma_reason, 0, sizeof(dev->dma_reason));
	dev->irq_mask = B43_IRQ_MASKTEMPLATE;
	if (b43_modparam_verbose < B43_VERBOSITY_DEBUG)
		dev->irq_mask &= ~B43_IRQ_PHY_TXERR;

	dev->mac_suspended = 1;

	/* Noise calculation context */
	memset(&dev->noisecalc, 0, sizeof(dev->noisecalc));
}