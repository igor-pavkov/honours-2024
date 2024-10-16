static inline int __set_dabr(unsigned long dabr, unsigned long dabrx)
{
	unsigned long addr = dabr & ~HW_BRK_TYPE_DABR;
	unsigned long lctrl1 = 0x90000000; /* compare type: equal on E & F */
	unsigned long lctrl2 = 0x8e000002; /* watchpoint 1 on cmp E | F */

	if ((dabr & HW_BRK_TYPE_RDWR) == HW_BRK_TYPE_READ)
		lctrl1 |= 0xa0000;
	else if ((dabr & HW_BRK_TYPE_RDWR) == HW_BRK_TYPE_WRITE)
		lctrl1 |= 0xf0000;
	else if ((dabr & HW_BRK_TYPE_RDWR) == 0)
		lctrl2 = 0;

	mtspr(SPRN_LCTRL2, 0);
	mtspr(SPRN_CMPE, addr);
	mtspr(SPRN_CMPF, addr + 4);
	mtspr(SPRN_LCTRL1, lctrl1);
	mtspr(SPRN_LCTRL2, lctrl2);

	return 0;
}