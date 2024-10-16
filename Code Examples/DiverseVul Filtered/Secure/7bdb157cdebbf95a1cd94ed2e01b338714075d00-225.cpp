void __perf_event_task_sched_out(struct task_struct *task,
				 struct task_struct *next)
{
	int ctxn;

	if (atomic_read(&nr_switch_events))
		perf_event_switch(task, next, false);

	for_each_task_context_nr(ctxn)
		perf_event_context_sched_out(task, ctxn, next);

	/*
	 * if cgroup events exist on this CPU, then we need
	 * to check if we have to switch out PMU state.
	 * cgroup event are system-wide mode only
	 */
	if (atomic_read(this_cpu_ptr(&perf_cgroup_events)))
		perf_cgroup_sched_out(task, next);
}