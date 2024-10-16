static inline bool perf_event_is_tracing(struct perf_event *event)
{
	if (event->pmu == &perf_tracepoint)
		return true;
#ifdef CONFIG_KPROBE_EVENTS
	if (event->pmu == &perf_kprobe)
		return true;
#endif
#ifdef CONFIG_UPROBE_EVENTS
	if (event->pmu == &perf_uprobe)
		return true;
#endif
	return false;
}