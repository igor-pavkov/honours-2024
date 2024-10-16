static void b43_set_slot_time(struct b43_wldev *dev, u16 slot_time)
{
	/* slot_time is in usec. */
	/* This test used to exit for all but a G PHY. */
	if (b43_current_band(dev->wl) == IEEE80211_BAND_5GHZ)
		return;
	b43_write16(dev, B43_MMIO_IFSSLOT, 510 + slot_time);
	/* Shared memory location 0x0010 is the slot time and should be
	 * set to slot_time; however, this register is initially 0 and changing
	 * the value adversely affects the transmit rate for BCM4311
	 * devices. Until this behavior is unterstood, delete this step
	 *
	 * b43_shm_write16(dev, B43_SHM_SHARED, 0x0010, slot_time);
	 */
}