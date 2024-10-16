static void vmx_set_cr0(struct kvm_vcpu *vcpu, unsigned long cr0)
{
	unsigned long hw_cr0 = (cr0 & ~KVM_GUEST_CR0_MASK) |
				KVM_VM_CR0_ALWAYS_ON;

	vmx_fpu_deactivate(vcpu);

	if (vcpu->arch.rmode.active && (cr0 & X86_CR0_PE))
		enter_pmode(vcpu);

	if (!vcpu->arch.rmode.active && !(cr0 & X86_CR0_PE))
		enter_rmode(vcpu);

#ifdef CONFIG_X86_64
	if (vcpu->arch.shadow_efer & EFER_LME) {
		if (!is_paging(vcpu) && (cr0 & X86_CR0_PG))
			enter_lmode(vcpu);
		if (is_paging(vcpu) && !(cr0 & X86_CR0_PG))
			exit_lmode(vcpu);
	}
#endif

	if (vm_need_ept())
		ept_update_paging_mode_cr0(&hw_cr0, cr0, vcpu);

	vmcs_writel(CR0_READ_SHADOW, cr0);
	vmcs_writel(GUEST_CR0, hw_cr0);
	vcpu->arch.cr0 = cr0;

	if (!(cr0 & X86_CR0_TS) || !(cr0 & X86_CR0_PE))
		vmx_fpu_activate(vcpu);
}