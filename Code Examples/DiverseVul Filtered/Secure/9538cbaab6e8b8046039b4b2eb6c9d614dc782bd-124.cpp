void b43_hf_write(struct b43_wldev *dev, u64 value)
{
	u16 lo, mi, hi;

	lo = (value & 0x00000000FFFFULL);
	mi = (value & 0x0000FFFF0000ULL) >> 16;
	hi = (value & 0xFFFF00000000ULL) >> 32;
	b43_shm_write16(dev, B43_SHM_SHARED, B43_SHM_SH_HOSTF1, lo);
	b43_shm_write16(dev, B43_SHM_SHARED, B43_SHM_SH_HOSTF2, mi);
	b43_shm_write16(dev, B43_SHM_SHARED, B43_SHM_SH_HOSTF3, hi);
}