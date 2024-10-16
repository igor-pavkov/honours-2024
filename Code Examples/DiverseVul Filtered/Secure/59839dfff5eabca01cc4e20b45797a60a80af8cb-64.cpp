static int dm_request_for_irq_injection(struct kvm_vcpu *vcpu,
					  struct kvm_run *kvm_run)
{
	return (!vcpu->arch.irq_summary &&
		kvm_run->request_interrupt_window &&
		vcpu->arch.interrupt_window_open &&
		(kvm_x86_ops->get_rflags(vcpu) & X86_EFLAGS_IF));
}