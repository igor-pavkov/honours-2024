static void vmx_set_rflags(struct kvm_vcpu *vcpu, unsigned long rflags)
{
	if (vcpu->arch.rmode.active)
		rflags |= X86_EFLAGS_IOPL | X86_EFLAGS_VM;
	vmcs_writel(GUEST_RFLAGS, rflags);
}