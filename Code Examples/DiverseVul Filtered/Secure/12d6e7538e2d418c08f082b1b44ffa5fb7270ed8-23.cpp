void kvm_vcpu_kick(struct kvm_vcpu *vcpu)
{
	int me;
	int cpu = vcpu->cpu;
	wait_queue_head_t *wqp;

	wqp = kvm_arch_vcpu_wq(vcpu);
	if (waitqueue_active(wqp)) {
		wake_up_interruptible(wqp);
		++vcpu->stat.halt_wakeup;
	}

	me = get_cpu();
	if (cpu != me && (unsigned)cpu < nr_cpu_ids && cpu_online(cpu))
		if (kvm_arch_vcpu_should_kick(vcpu))
			smp_send_reschedule(cpu);
	put_cpu();
}