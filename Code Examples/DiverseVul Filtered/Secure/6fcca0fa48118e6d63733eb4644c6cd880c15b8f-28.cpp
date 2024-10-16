static void group_init(struct psi_group *group)
{
	int cpu;

	for_each_possible_cpu(cpu)
		seqcount_init(&per_cpu_ptr(group->pcpu, cpu)->seq);
	group->avg_last_update = sched_clock();
	group->avg_next_update = group->avg_last_update + psi_period;
	INIT_DELAYED_WORK(&group->avgs_work, psi_avgs_work);
	mutex_init(&group->avgs_lock);
	/* Init trigger-related members */
	atomic_set(&group->poll_scheduled, 0);
	mutex_init(&group->trigger_lock);
	INIT_LIST_HEAD(&group->triggers);
	memset(group->nr_triggers, 0, sizeof(group->nr_triggers));
	group->poll_states = 0;
	group->poll_min_period = U32_MAX;
	memset(group->polling_total, 0, sizeof(group->polling_total));
	group->polling_next_update = ULLONG_MAX;
	group->polling_until = 0;
	rcu_assign_pointer(group->poll_kworker, NULL);
}