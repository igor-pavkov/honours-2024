static int handle_vmcall(struct kvm_vcpu *vcpu, struct kvm_run *kvm_run)
{
	skip_emulated_instruction(vcpu);
	kvm_emulate_hypercall(vcpu);
	return 1;
}