static int handle_interrupt_window(struct kvm_vcpu *vcpu)
{
	exec_controls_clearbit(to_vmx(vcpu), CPU_BASED_INTR_WINDOW_EXITING);

	kvm_make_request(KVM_REQ_EVENT, vcpu);

	++vcpu->stat.irq_window_exits;
	return 1;
}