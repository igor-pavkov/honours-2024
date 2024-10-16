static void prepare_vmcs02_constant_state(struct vcpu_vmx *vmx)
{
	/*
	 * If vmcs02 hasn't been initialized, set the constant vmcs02 state
	 * according to L0's settings (vmcs12 is irrelevant here).  Host
	 * fields that come from L0 and are not constant, e.g. HOST_CR3,
	 * will be set as needed prior to VMLAUNCH/VMRESUME.
	 */
	if (vmx->nested.vmcs02_initialized)
		return;
	vmx->nested.vmcs02_initialized = true;

	/*
	 * We don't care what the EPTP value is we just need to guarantee
	 * it's valid so we don't get a false positive when doing early
	 * consistency checks.
	 */
	if (enable_ept && nested_early_check)
		vmcs_write64(EPT_POINTER, construct_eptp(&vmx->vcpu, 0));

	/* All VMFUNCs are currently emulated through L0 vmexits.  */
	if (cpu_has_vmx_vmfunc())
		vmcs_write64(VM_FUNCTION_CONTROL, 0);

	if (cpu_has_vmx_posted_intr())
		vmcs_write16(POSTED_INTR_NV, POSTED_INTR_NESTED_VECTOR);

	if (cpu_has_vmx_msr_bitmap())
		vmcs_write64(MSR_BITMAP, __pa(vmx->nested.vmcs02.msr_bitmap));

	if (enable_pml)
		vmcs_write64(PML_ADDRESS, page_to_phys(vmx->pml_pg));

	/*
	 * Set the MSR load/store lists to match L0's settings.  Only the
	 * addresses are constant (for vmcs02), the counts can change based
	 * on L2's behavior, e.g. switching to/from long mode.
	 */
	vmcs_write32(VM_EXIT_MSR_STORE_COUNT, 0);
	vmcs_write64(VM_EXIT_MSR_LOAD_ADDR, __pa(vmx->msr_autoload.host.val));
	vmcs_write64(VM_ENTRY_MSR_LOAD_ADDR, __pa(vmx->msr_autoload.guest.val));

	vmx_set_constant_host_state(vmx);
}