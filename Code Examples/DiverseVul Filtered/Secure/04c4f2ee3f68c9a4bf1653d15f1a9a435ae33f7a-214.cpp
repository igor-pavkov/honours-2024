static void pt_guest_enter(struct vcpu_vmx *vmx)
{
	if (vmx_pt_mode_is_system())
		return;

	/*
	 * GUEST_IA32_RTIT_CTL is already set in the VMCS.
	 * Save host state before VM entry.
	 */
	rdmsrl(MSR_IA32_RTIT_CTL, vmx->pt_desc.host.ctl);
	if (vmx->pt_desc.guest.ctl & RTIT_CTL_TRACEEN) {
		wrmsrl(MSR_IA32_RTIT_CTL, 0);
		pt_save_msr(&vmx->pt_desc.host, vmx->pt_desc.addr_range);
		pt_load_msr(&vmx->pt_desc.guest, vmx->pt_desc.addr_range);
	}
}