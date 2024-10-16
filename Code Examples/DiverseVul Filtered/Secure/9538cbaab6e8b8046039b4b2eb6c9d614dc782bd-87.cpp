static void b43_time_lock(struct b43_wldev *dev)
{
	b43_maskset32(dev, B43_MMIO_MACCTL, ~0, B43_MACCTL_TBTTHOLD);
	/* Commit the write */
	b43_read32(dev, B43_MMIO_MACCTL);
}