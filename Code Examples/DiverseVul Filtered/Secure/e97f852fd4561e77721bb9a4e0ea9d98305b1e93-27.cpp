static void kvm_set_mmio_spte_mask(void)
{
	u64 mask;
	int maxphyaddr = boot_cpu_data.x86_phys_bits;

	/*
	 * Set the reserved bits and the present bit of an paging-structure
	 * entry to generate page fault with PFER.RSV = 1.
	 */

	/*
	 * Mask the uppermost physical address bit, which would be reserved as
	 * long as the supported physical address width is less than 52.
	 */
	mask = 1ull << 51;

	/* Set the present bit. */
	mask |= 1ull;

	/*
	 * If reserved bit is not supported, clear the present bit to disable
	 * mmio page fault.
	 */
	if (IS_ENABLED(CONFIG_X86_64) && maxphyaddr == 52)
		mask &= ~1ull;

	kvm_mmu_set_mmio_spte_mask(mask, mask);
}