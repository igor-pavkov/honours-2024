static int handle_preemption_timer(struct kvm_vcpu *vcpu)
{
	handle_fastpath_preemption_timer(vcpu);
	return 1;
}