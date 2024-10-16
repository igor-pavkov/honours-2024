static void b43_ssb_wireless_core_reset(struct b43_wldev *dev, bool gmode)
{
	struct ssb_device *sdev = dev->dev->sdev;
	u32 tmslow;
	u32 flags = 0;

	if (gmode)
		flags |= B43_TMSLOW_GMODE;
	flags |= B43_TMSLOW_PHYCLKEN;
	flags |= B43_TMSLOW_PHYRESET;
	if (dev->phy.type == B43_PHYTYPE_N)
		flags |= B43_TMSLOW_PHY_BANDWIDTH_20MHZ; /* Make 20 MHz def */
	b43_device_enable(dev, flags);
	msleep(2);		/* Wait for the PLL to turn on. */

	/* Now take the PHY out of Reset again */
	tmslow = ssb_read32(sdev, SSB_TMSLOW);
	tmslow |= SSB_TMSLOW_FGC;
	tmslow &= ~B43_TMSLOW_PHYRESET;
	ssb_write32(sdev, SSB_TMSLOW, tmslow);
	ssb_read32(sdev, SSB_TMSLOW);	/* flush */
	msleep(1);
	tmslow &= ~SSB_TMSLOW_FGC;
	ssb_write32(sdev, SSB_TMSLOW, tmslow);
	ssb_read32(sdev, SSB_TMSLOW);	/* flush */
	msleep(1);
}