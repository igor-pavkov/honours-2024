static inline u64 nested_vmx_get_vmcs01_guest_efer(struct vcpu_vmx *vmx)
{
	struct shared_msr_entry *efer_msr;
	unsigned int i;

	if (vm_entry_controls_get(vmx) & VM_ENTRY_LOAD_IA32_EFER)
		return vmcs_read64(GUEST_IA32_EFER);

	if (cpu_has_load_ia32_efer())
		return host_efer;

	for (i = 0; i < vmx->msr_autoload.guest.nr; ++i) {
		if (vmx->msr_autoload.guest.val[i].index == MSR_EFER)
			return vmx->msr_autoload.guest.val[i].value;
	}

	efer_msr = find_msr_entry(vmx, MSR_EFER);
	if (efer_msr)
		return efer_msr->data;

	return host_efer;
}