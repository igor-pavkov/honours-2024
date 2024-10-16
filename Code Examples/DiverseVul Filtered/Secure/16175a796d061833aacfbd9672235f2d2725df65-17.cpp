static int handle_halt(struct kvm_vcpu *vcpu, struct kvm_run *kvm_run)
{
	skip_emulated_instruction(vcpu);
	return kvm_emulate_halt(vcpu);
}