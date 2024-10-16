static void ept_update_paging_mode_cr0(unsigned long *hw_cr0,
					unsigned long cr0,
					struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	if (!kvm_register_is_available(vcpu, VCPU_EXREG_CR3))
		vmx_cache_reg(vcpu, VCPU_EXREG_CR3);
	if (!(cr0 & X86_CR0_PG)) {
		/* From paging/starting to nonpaging */
		exec_controls_setbit(vmx, CPU_BASED_CR3_LOAD_EXITING |
					  CPU_BASED_CR3_STORE_EXITING);
		vcpu->arch.cr0 = cr0;
		vmx_set_cr4(vcpu, kvm_read_cr4(vcpu));
	} else if (!is_paging(vcpu)) {
		/* From nonpaging to paging */
		exec_controls_clearbit(vmx, CPU_BASED_CR3_LOAD_EXITING |
					    CPU_BASED_CR3_STORE_EXITING);
		vcpu->arch.cr0 = cr0;
		vmx_set_cr4(vcpu, kvm_read_cr4(vcpu));
	}

	if (!(cr0 & X86_CR0_WP))
		*hw_cr0 &= ~X86_CR0_WP;
}