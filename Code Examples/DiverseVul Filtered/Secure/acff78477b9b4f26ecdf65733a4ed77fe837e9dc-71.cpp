static int nested_vmx_check_vmentry_postreqs(struct kvm_vcpu *vcpu,
					     struct vmcs12 *vmcs12,
					     u32 *exit_qual)
{
	bool ia32e;

	*exit_qual = ENTRY_FAIL_DEFAULT;

	if (!nested_guest_cr0_valid(vcpu, vmcs12->guest_cr0) ||
	    !nested_guest_cr4_valid(vcpu, vmcs12->guest_cr4))
		return 1;

	if (nested_vmx_check_vmcs_link_ptr(vcpu, vmcs12)) {
		*exit_qual = ENTRY_FAIL_VMCS_LINK_PTR;
		return 1;
	}

	/*
	 * If the load IA32_EFER VM-entry control is 1, the following checks
	 * are performed on the field for the IA32_EFER MSR:
	 * - Bits reserved in the IA32_EFER MSR must be 0.
	 * - Bit 10 (corresponding to IA32_EFER.LMA) must equal the value of
	 *   the IA-32e mode guest VM-exit control. It must also be identical
	 *   to bit 8 (LME) if bit 31 in the CR0 field (corresponding to
	 *   CR0.PG) is 1.
	 */
	if (to_vmx(vcpu)->nested.nested_run_pending &&
	    (vmcs12->vm_entry_controls & VM_ENTRY_LOAD_IA32_EFER)) {
		ia32e = (vmcs12->vm_entry_controls & VM_ENTRY_IA32E_MODE) != 0;
		if (!kvm_valid_efer(vcpu, vmcs12->guest_ia32_efer) ||
		    ia32e != !!(vmcs12->guest_ia32_efer & EFER_LMA) ||
		    ((vmcs12->guest_cr0 & X86_CR0_PG) &&
		     ia32e != !!(vmcs12->guest_ia32_efer & EFER_LME)))
			return 1;
	}

	if ((vmcs12->vm_entry_controls & VM_ENTRY_LOAD_BNDCFGS) &&
		(is_noncanonical_address(vmcs12->guest_bndcfgs & PAGE_MASK, vcpu) ||
		(vmcs12->guest_bndcfgs & MSR_IA32_BNDCFGS_RSVD)))
			return 1;

	return 0;
}