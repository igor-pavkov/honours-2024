static bool emulation_required(struct kvm_vcpu *vcpu)
{
	return emulate_invalid_guest_state && !vmx_guest_state_valid(vcpu);
}