static inline unsigned char fdc_inb(int fdc, int reg)
{
	return fd_inb(fdc_state[fdc].address, reg);
}