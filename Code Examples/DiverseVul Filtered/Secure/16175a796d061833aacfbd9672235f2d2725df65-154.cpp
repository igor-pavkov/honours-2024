static void reload_host_efer(struct vcpu_vmx *vmx)
{
	if (vmx->host_state.guest_efer_loaded) {
		vmx->host_state.guest_efer_loaded = 0;
		load_msrs(vmx->host_msrs + vmx->msr_offset_efer, 1);
	}
}