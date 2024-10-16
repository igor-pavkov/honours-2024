static u32 b43_jssi_read(struct b43_wldev *dev)
{
	u32 val = 0;

	val = b43_shm_read16(dev, B43_SHM_SHARED, B43_SHM_SH_JSSI1);
	val <<= 16;
	val |= b43_shm_read16(dev, B43_SHM_SHARED, B43_SHM_SH_JSSI0);

	return val;
}