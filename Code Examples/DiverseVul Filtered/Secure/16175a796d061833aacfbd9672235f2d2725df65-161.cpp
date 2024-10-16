static void vmx_fpu_deactivate(struct kvm_vcpu *vcpu)
{
	if (!vcpu->fpu_active)
		return;
	vcpu->fpu_active = 0;
	vmcs_set_bits(GUEST_CR0, X86_CR0_TS);
	update_exception_bitmap(vcpu);
}