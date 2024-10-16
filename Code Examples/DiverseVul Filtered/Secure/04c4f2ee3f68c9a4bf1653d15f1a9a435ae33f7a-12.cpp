static int vmx_skip_emulated_instruction(struct kvm_vcpu *vcpu)
{
	vmx_update_emulated_instruction(vcpu);
	return skip_emulated_instruction(vcpu);
}