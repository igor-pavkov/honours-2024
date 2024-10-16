static void b43_jssi_write(struct b43_wldev *dev, u32 jssi)
{
	b43_shm_write16(dev, B43_SHM_SHARED, B43_SHM_SH_JSSI0,
			(jssi & 0x0000FFFF));
	b43_shm_write16(dev, B43_SHM_SHARED, B43_SHM_SH_JSSI1,
			(jssi & 0xFFFF0000) >> 16);
}