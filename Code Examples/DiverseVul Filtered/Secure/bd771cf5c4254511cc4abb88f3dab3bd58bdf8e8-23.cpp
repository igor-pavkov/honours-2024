static inline void sm7xx_init_hw(void)
{
	outb_p(0x18, 0x3c4);
	outb_p(0x11, 0x3c5);
}