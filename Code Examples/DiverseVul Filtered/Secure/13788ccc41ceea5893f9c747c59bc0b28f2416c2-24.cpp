static void migrate_hrtimers(int cpu)
{
	struct hrtimer_cpu_base *old_base, *new_base;
	int i;

	BUG_ON(cpu_online(cpu));
	old_base = &per_cpu(hrtimer_bases, cpu);
	new_base = &get_cpu_var(hrtimer_bases);

	tick_cancel_sched_timer(cpu);

	local_irq_disable();
	double_spin_lock(&new_base->lock, &old_base->lock,
			 smp_processor_id() < cpu);

	for (i = 0; i < HRTIMER_MAX_CLOCK_BASES; i++) {
		migrate_hrtimer_list(&old_base->clock_base[i],
				     &new_base->clock_base[i]);
	}

	double_spin_unlock(&new_base->lock, &old_base->lock,
			   smp_processor_id() < cpu);
	local_irq_enable();
	put_cpu_var(hrtimer_bases);
}