static void b43_set_synth_pu_delay(struct b43_wldev *dev, bool idle)
{
	u16 pu_delay;

	/* The time value is in microseconds. */
	if (dev->phy.type == B43_PHYTYPE_A)
		pu_delay = 3700;
	else
		pu_delay = 1050;
	if (b43_is_mode(dev->wl, NL80211_IFTYPE_ADHOC) || idle)
		pu_delay = 500;
	if ((dev->phy.radio_ver == 0x2050) && (dev->phy.radio_rev == 8))
		pu_delay = max(pu_delay, (u16)2400);

	b43_shm_write16(dev, B43_SHM_SHARED, B43_SHM_SH_SPUWKUP, pu_delay);
}