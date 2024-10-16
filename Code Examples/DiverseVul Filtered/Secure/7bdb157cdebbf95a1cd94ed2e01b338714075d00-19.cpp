static int perf_kprobe_event_init(struct perf_event *event)
{
	int err;
	bool is_retprobe;

	if (event->attr.type != perf_kprobe.type)
		return -ENOENT;

	if (!perfmon_capable())
		return -EACCES;

	/*
	 * no branch sampling for probe events
	 */
	if (has_branch_stack(event))
		return -EOPNOTSUPP;

	is_retprobe = event->attr.config & PERF_PROBE_CONFIG_IS_RETPROBE;
	err = perf_kprobe_init(event, is_retprobe);
	if (err)
		return err;

	event->destroy = perf_kprobe_destroy;

	return 0;
}