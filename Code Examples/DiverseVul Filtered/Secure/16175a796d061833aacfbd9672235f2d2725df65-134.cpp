static int handle_external_interrupt(struct kvm_vcpu *vcpu,
				     struct kvm_run *kvm_run)
{
	++vcpu->stat.irq_exits;
	KVMTRACE_1D(INTR, vcpu, vmcs_read32(VM_EXIT_INTR_INFO), handler);
	return 1;
}