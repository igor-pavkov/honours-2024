void kvm_vcpu_kick(struct kvm_vcpu *vcpu)
{
	int ipi_pcpu = vcpu->cpu;
	int cpu = get_cpu();

	if (waitqueue_active(&vcpu->wq)) {
		wake_up_interruptible(&vcpu->wq);
		++vcpu->stat.halt_wakeup;
	}
	/*
	 * We may be called synchronously with irqs disabled in guest mode,
	 * So need not to call smp_call_function_single() in that case.
	 */
	if (vcpu->guest_mode && vcpu->cpu != cpu)
		smp_call_function_single(ipi_pcpu, vcpu_kick_intr, vcpu, 0);
	put_cpu();
}