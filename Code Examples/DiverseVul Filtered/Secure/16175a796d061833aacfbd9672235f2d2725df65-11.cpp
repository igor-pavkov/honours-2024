static int handle_cpuid(struct kvm_vcpu *vcpu, struct kvm_run *kvm_run)
{
	kvm_emulate_cpuid(vcpu);
	return 1;
}