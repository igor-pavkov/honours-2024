static void pt_guest_exit(struct vcpu_vmx *vmx)
{
	if (vmx_pt_mode_is_system())
		return;

	if (vmx->pt_desc.guest.ctl & RTIT_CTL_TRACEEN) {
		pt_save_msr(&vmx->pt_desc.guest, vmx->pt_desc.addr_range);
		pt_load_msr(&vmx->pt_desc.host, vmx->pt_desc.addr_range);
	}

	/* Reload host state (IA32_RTIT_CTL will be cleared on VM exit). */
	wrmsrl(MSR_IA32_RTIT_CTL, vmx->pt_desc.host.ctl);
}