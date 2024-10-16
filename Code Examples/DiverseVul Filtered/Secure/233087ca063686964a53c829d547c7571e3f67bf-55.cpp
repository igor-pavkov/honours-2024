static inline void fdc_outb(unsigned char value, int fdc, int reg)
{
	fd_outb(value, fdc_state[fdc].address, reg);
}