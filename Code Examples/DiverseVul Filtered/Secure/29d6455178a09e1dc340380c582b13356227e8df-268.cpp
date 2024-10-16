int cpuset_cpumask_can_shrink(const struct cpumask *cur,
			      const struct cpumask *trial)
{
	int ret = 1, trial_cpus;
	struct dl_bw *cur_dl_b;
	unsigned long flags;

	if (!cpumask_weight(cur))
		return ret;

	rcu_read_lock_sched();
	cur_dl_b = dl_bw_of(cpumask_any(cur));
	trial_cpus = cpumask_weight(trial);

	raw_spin_lock_irqsave(&cur_dl_b->lock, flags);
	if (cur_dl_b->bw != -1 &&
	    cur_dl_b->bw * trial_cpus < cur_dl_b->total_bw)
		ret = 0;
	raw_spin_unlock_irqrestore(&cur_dl_b->lock, flags);
	rcu_read_unlock_sched();

	return ret;
}