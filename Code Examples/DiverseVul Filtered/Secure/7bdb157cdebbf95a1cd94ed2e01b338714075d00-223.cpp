void perf_event_itrace_started(struct perf_event *event)
{
	event->attach_state |= PERF_ATTACH_ITRACE;
}