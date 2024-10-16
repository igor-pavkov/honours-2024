static int handle_nmi_window(struct kvm_vcpu *vcpu)
{
	WARN_ON_ONCE(!enable_vnmi);
	exec_controls_clearbit(to_vmx(vcpu), CPU_BASED_NMI_WINDOW_EXITING);
	++vcpu->stat.nmi_window_exits;
	kvm_make_request(KVM_REQ_EVENT, vcpu);

	return 1;
}