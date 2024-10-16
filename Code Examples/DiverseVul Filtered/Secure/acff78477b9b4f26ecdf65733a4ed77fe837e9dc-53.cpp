static int nested_check_host_control_regs(struct kvm_vcpu *vcpu,
                                          struct vmcs12 *vmcs12)
{
	bool ia32e;

	if (!nested_host_cr0_valid(vcpu, vmcs12->host_cr0) ||
	    !nested_host_cr4_valid(vcpu, vmcs12->host_cr4) ||
	    !nested_cr3_valid(vcpu, vmcs12->host_cr3))
		return -EINVAL;

	if (is_noncanonical_address(vmcs12->host_ia32_sysenter_esp, vcpu) ||
	    is_noncanonical_address(vmcs12->host_ia32_sysenter_eip, vcpu))
		return -EINVAL;

	/*
	 * If the load IA32_EFER VM-exit control is 1, bits reserved in the
	 * IA32_EFER MSR must be 0 in the field for that register. In addition,
	 * the values of the LMA and LME bits in the field must each be that of
	 * the host address-space size VM-exit control.
	 */
	if (vmcs12->vm_exit_controls & VM_EXIT_LOAD_IA32_EFER) {
		ia32e = (vmcs12->vm_exit_controls &
			 VM_EXIT_HOST_ADDR_SPACE_SIZE) != 0;
		if (!kvm_valid_efer(vcpu, vmcs12->host_ia32_efer) ||
		    ia32e != !!(vmcs12->host_ia32_efer & EFER_LMA) ||
		    ia32e != !!(vmcs12->host_ia32_efer & EFER_LME))
			return -EINVAL;
	}

	return 0;
}