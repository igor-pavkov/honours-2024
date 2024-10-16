static void vmx_clear_msr_bitmap_write(ulong *msr_bitmap, u32 msr)
{
	int f = sizeof(unsigned long);

	if (msr <= 0x1fff)
		__clear_bit(msr, msr_bitmap + 0x800 / f);
	else if ((msr >= 0xc0000000) && (msr <= 0xc0001fff))
		__clear_bit(msr & 0x1fff, msr_bitmap + 0xc00 / f);
}