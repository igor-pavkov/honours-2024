static void ept_set_mmio_spte_mask(void)
{
	/*
	 * EPT Misconfigurations can be generated if the value of bits 2:0
	 * of an EPT paging-structure entry is 110b (write/execute).
	 */
	kvm_mmu_set_mmio_spte_mask(VMX_EPT_MISCONFIG_WX_VALUE, 0);
}