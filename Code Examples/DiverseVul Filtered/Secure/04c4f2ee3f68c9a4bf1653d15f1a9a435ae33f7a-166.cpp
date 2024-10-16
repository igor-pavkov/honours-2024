static int handle_bus_lock_vmexit(struct kvm_vcpu *vcpu)
{
	vcpu->run->exit_reason = KVM_EXIT_X86_BUS_LOCK;
	vcpu->run->flags |= KVM_RUN_X86_BUS_LOCK;
	return 0;
}