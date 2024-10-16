void vmx_set_rflags(struct kvm_vcpu *vcpu, unsigned long rflags)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	unsigned long old_rflags;

	if (is_unrestricted_guest(vcpu)) {
		kvm_register_mark_available(vcpu, VCPU_EXREG_RFLAGS);
		vmx->rflags = rflags;
		vmcs_writel(GUEST_RFLAGS, rflags);
		return;
	}

	old_rflags = vmx_get_rflags(vcpu);
	vmx->rflags = rflags;
	if (vmx->rmode.vm86_active) {
		vmx->rmode.save_rflags = rflags;
		rflags |= X86_EFLAGS_IOPL | X86_EFLAGS_VM;
	}
	vmcs_writel(GUEST_RFLAGS, rflags);

	if ((old_rflags ^ vmx->rflags) & X86_EFLAGS_VM)
		vmx->emulation_required = emulation_required(vcpu);
}