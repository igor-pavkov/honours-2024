static u64 nested_vmx_calc_efer(struct vcpu_vmx *vmx, struct vmcs12 *vmcs12)
{
	if (vmx->nested.nested_run_pending &&
	    (vmcs12->vm_entry_controls & VM_ENTRY_LOAD_IA32_EFER))
		return vmcs12->guest_ia32_efer;
	else if (vmcs12->vm_entry_controls & VM_ENTRY_IA32E_MODE)
		return vmx->vcpu.arch.efer | (EFER_LMA | EFER_LME);
	else
		return vmx->vcpu.arch.efer & ~(EFER_LMA | EFER_LME);
}