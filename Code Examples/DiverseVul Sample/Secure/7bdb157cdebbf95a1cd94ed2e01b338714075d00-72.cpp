static u32 perf_event_pid_type(struct perf_event *event, struct task_struct *p,
				enum pid_type type)
{
	u32 nr;
	/*
	 * only top level events have the pid namespace they were created in
	 */
	if (event->parent)
		event = event->parent;

	nr = __task_pid_nr_ns(p, type, event->ns);
	/* avoid -1 if it is idle thread or runs in another ns */
	if (!nr && !pid_alive(p))
		nr = -1;
	return nr;
}