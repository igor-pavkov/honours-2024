void b43_wireless_core_reset(struct b43_wldev *dev, bool gmode)
{
	u32 macctl;

	switch (dev->dev->bus_type) {
#ifdef CONFIG_B43_BCMA
	case B43_BUS_BCMA:
		b43_bcma_wireless_core_reset(dev, gmode);
		break;
#endif
#ifdef CONFIG_B43_SSB
	case B43_BUS_SSB:
		b43_ssb_wireless_core_reset(dev, gmode);
		break;
#endif
	}

	/* Turn Analog ON, but only if we already know the PHY-type.
	 * This protects against very early setup where we don't know the
	 * PHY-type, yet. wireless_core_reset will be called once again later,
	 * when we know the PHY-type. */
	if (dev->phy.ops)
		dev->phy.ops->switch_analog(dev, 1);

	macctl = b43_read32(dev, B43_MMIO_MACCTL);
	macctl &= ~B43_MACCTL_GMODE;
	if (gmode)
		macctl |= B43_MACCTL_GMODE;
	macctl |= B43_MACCTL_IHR_ENABLED;
	b43_write32(dev, B43_MMIO_MACCTL, macctl);
}