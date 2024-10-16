int perf_event_account_interrupt(struct perf_event *event)
{
	return __perf_event_account_interrupt(event, 1);
}