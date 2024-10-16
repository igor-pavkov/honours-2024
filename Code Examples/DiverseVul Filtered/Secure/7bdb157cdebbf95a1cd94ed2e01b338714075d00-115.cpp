static void __init perf_event_init_all_cpus(void)
{
	struct swevent_htable *swhash;
	int cpu;

	zalloc_cpumask_var(&perf_online_mask, GFP_KERNEL);

	for_each_possible_cpu(cpu) {
		swhash = &per_cpu(swevent_htable, cpu);
		mutex_init(&swhash->hlist_mutex);
		INIT_LIST_HEAD(&per_cpu(active_ctx_list, cpu));

		INIT_LIST_HEAD(&per_cpu(pmu_sb_events.list, cpu));
		raw_spin_lock_init(&per_cpu(pmu_sb_events.lock, cpu));

#ifdef CONFIG_CGROUP_PERF
		INIT_LIST_HEAD(&per_cpu(cgrp_cpuctx_list, cpu));
#endif
	}
}