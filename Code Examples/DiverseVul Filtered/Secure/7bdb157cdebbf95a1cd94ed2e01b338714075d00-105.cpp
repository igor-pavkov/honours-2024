static void unaccount_event(struct perf_event *event)
{
	bool dec = false;

	if (event->parent)
		return;

	if (event->attach_state & PERF_ATTACH_TASK)
		dec = true;
	if (event->attr.mmap || event->attr.mmap_data)
		atomic_dec(&nr_mmap_events);
	if (event->attr.comm)
		atomic_dec(&nr_comm_events);
	if (event->attr.namespaces)
		atomic_dec(&nr_namespaces_events);
	if (event->attr.cgroup)
		atomic_dec(&nr_cgroup_events);
	if (event->attr.task)
		atomic_dec(&nr_task_events);
	if (event->attr.freq)
		unaccount_freq_event();
	if (event->attr.context_switch) {
		dec = true;
		atomic_dec(&nr_switch_events);
	}
	if (is_cgroup_event(event))
		dec = true;
	if (has_branch_stack(event))
		dec = true;
	if (event->attr.ksymbol)
		atomic_dec(&nr_ksymbol_events);
	if (event->attr.bpf_event)
		atomic_dec(&nr_bpf_events);
	if (event->attr.text_poke)
		atomic_dec(&nr_text_poke_events);

	if (dec) {
		if (!atomic_add_unless(&perf_sched_count, -1, 1))
			schedule_delayed_work(&perf_sched_work, HZ);
	}

	unaccount_event_cpu(event, event->cpu);

	unaccount_pmu_sb_event(event);
}