int kvm_emulate_instruction(struct kvm_vcpu *vcpu, int emulation_type)
{
	return x86_emulate_instruction(vcpu, 0, emulation_type, NULL, 0);
}