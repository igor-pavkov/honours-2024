int vcpu_load(struct kvm_vcpu *vcpu)
{
	int cpu;

	if (mutex_lock_killable(&vcpu->mutex))
		return -EINTR;
	if (unlikely(vcpu->pid != current->pids[PIDTYPE_PID].pid)) {
		/* The thread running this VCPU changed. */
		struct pid *oldpid = vcpu->pid;
		struct pid *newpid = get_task_pid(current, PIDTYPE_PID);
		rcu_assign_pointer(vcpu->pid, newpid);
		synchronize_rcu();
		put_pid(oldpid);
	}
	cpu = get_cpu();
	preempt_notifier_register(&vcpu->preempt_notifier);
	kvm_arch_vcpu_load(vcpu, cpu);
	put_cpu();
	return 0;
}