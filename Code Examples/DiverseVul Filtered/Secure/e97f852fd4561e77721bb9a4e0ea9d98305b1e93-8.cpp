static void kvm_put_guest_fpu(struct kvm_vcpu *vcpu)
{
	preempt_disable();
	copy_fpregs_to_fpstate(&vcpu->arch.guest_fpu);
	copy_kernel_to_fpregs(&vcpu->arch.user_fpu.state);
	preempt_enable();
	++vcpu->stat.fpu_reload;
	trace_kvm_fpu(0);
}