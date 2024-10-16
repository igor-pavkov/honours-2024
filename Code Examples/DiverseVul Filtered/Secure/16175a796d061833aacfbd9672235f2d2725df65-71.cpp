static void vmx_disable_intercept_for_msr(struct page *msr_bitmap, u32 msr)
{
	void *va;

	if (!cpu_has_vmx_msr_bitmap())
		return;

	/*
	 * See Intel PRM Vol. 3, 20.6.9 (MSR-Bitmap Address). Early manuals
	 * have the write-low and read-high bitmap offsets the wrong way round.
	 * We can control MSRs 0x00000000-0x00001fff and 0xc0000000-0xc0001fff.
	 */
	va = kmap(msr_bitmap);
	if (msr <= 0x1fff) {
		__clear_bit(msr, va + 0x000); /* read-low */
		__clear_bit(msr, va + 0x800); /* write-low */
	} else if ((msr >= 0xc0000000) && (msr <= 0xc0001fff)) {
		msr &= 0x1fff;
		__clear_bit(msr, va + 0x400); /* read-high */
		__clear_bit(msr, va + 0xc00); /* write-high */
	}
	kunmap(msr_bitmap);
}