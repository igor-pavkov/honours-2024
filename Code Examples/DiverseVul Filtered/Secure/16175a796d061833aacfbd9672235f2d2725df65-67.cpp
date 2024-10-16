static void vmx_fpu_activate(struct kvm_vcpu *vcpu)
{
	if (vcpu->fpu_active)
		return;
	vcpu->fpu_active = 1;
	vmcs_clear_bits(GUEST_CR0, X86_CR0_TS);
	if (vcpu->arch.cr0 & X86_CR0_TS)
		vmcs_set_bits(GUEST_CR0, X86_CR0_TS);
	update_exception_bitmap(vcpu);
}