int get_nohz_timer_target(void)
{
	int i, cpu = smp_processor_id();
	struct sched_domain *sd;

	if (!idle_cpu(cpu) && is_housekeeping_cpu(cpu))
		return cpu;

	rcu_read_lock();
	for_each_domain(cpu, sd) {
		for_each_cpu(i, sched_domain_span(sd)) {
			if (!idle_cpu(i) && is_housekeeping_cpu(cpu)) {
				cpu = i;
				goto unlock;
			}
		}
	}

	if (!is_housekeeping_cpu(cpu))
		cpu = housekeeping_any_cpu();
unlock:
	rcu_read_unlock();
	return cpu;
}