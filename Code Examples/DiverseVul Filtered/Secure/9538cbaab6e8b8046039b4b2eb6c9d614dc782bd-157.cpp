static void b43_bcma_wireless_core_reset(struct b43_wldev *dev, bool gmode)
{
	u32 req = B43_BCMA_CLKCTLST_80211_PLL_REQ |
		  B43_BCMA_CLKCTLST_PHY_PLL_REQ;
	u32 status = B43_BCMA_CLKCTLST_80211_PLL_ST |
		     B43_BCMA_CLKCTLST_PHY_PLL_ST;

	b43_device_enable(dev, B43_BCMA_IOCTL_PHY_CLKEN);
	bcma_core_set_clockmode(dev->dev->bdev, BCMA_CLKMODE_FAST);
	b43_bcma_phy_reset(dev);
	bcma_core_pll_ctl(dev->dev->bdev, req, status, true);
}