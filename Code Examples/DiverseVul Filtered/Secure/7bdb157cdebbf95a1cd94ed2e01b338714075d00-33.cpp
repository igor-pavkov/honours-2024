static int perf_tp_event_match(struct perf_event *event,
				struct perf_sample_data *data,
				struct pt_regs *regs)
{
	if (event->hw.state & PERF_HES_STOPPED)
		return 0;
	/*
	 * If exclude_kernel, only trace user-space tracepoints (uprobes)
	 */
	if (event->attr.exclude_kernel && !user_mode(regs))
		return 0;

	if (!perf_tp_filter_match(event, data))
		return 0;

	return 1;
}