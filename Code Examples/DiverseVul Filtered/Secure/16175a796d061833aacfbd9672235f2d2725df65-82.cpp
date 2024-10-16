static int handle_triple_fault(struct kvm_vcpu *vcpu, struct kvm_run *kvm_run)
{
	kvm_run->exit_reason = KVM_EXIT_SHUTDOWN;
	return 0;
}