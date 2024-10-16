static int cpuset_cpu_inactive(struct notifier_block *nfb, unsigned long action,
			       void *hcpu)
{
	unsigned long flags;
	long cpu = (long)hcpu;
	struct dl_bw *dl_b;
	bool overflow;
	int cpus;

	switch (action) {
	case CPU_DOWN_PREPARE:
		rcu_read_lock_sched();
		dl_b = dl_bw_of(cpu);

		raw_spin_lock_irqsave(&dl_b->lock, flags);
		cpus = dl_bw_cpus(cpu);
		overflow = __dl_overflow(dl_b, cpus, 0, 0);
		raw_spin_unlock_irqrestore(&dl_b->lock, flags);

		rcu_read_unlock_sched();

		if (overflow)
			return notifier_from_errno(-EBUSY);
		cpuset_update_active_cpus(false);
		break;
	case CPU_DOWN_PREPARE_FROZEN:
		num_cpus_frozen++;
		partition_sched_domains(1, NULL, NULL);
		break;
	default:
		return NOTIFY_DONE;
	}
	return NOTIFY_OK;
}