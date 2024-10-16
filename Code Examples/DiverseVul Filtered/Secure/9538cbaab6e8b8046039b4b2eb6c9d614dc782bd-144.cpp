static void b43_set_pretbtt(struct b43_wldev *dev)
{
	u16 pretbtt;

	/* The time value is in microseconds. */
	if (b43_is_mode(dev->wl, NL80211_IFTYPE_ADHOC)) {
		pretbtt = 2;
	} else {
		if (dev->phy.type == B43_PHYTYPE_A)
			pretbtt = 120;
		else
			pretbtt = 250;
	}
	b43_shm_write16(dev, B43_SHM_SHARED, B43_SHM_SH_PRETBTT, pretbtt);
	b43_write16(dev, B43_MMIO_TSF_CFP_PRETBTT, pretbtt);
}