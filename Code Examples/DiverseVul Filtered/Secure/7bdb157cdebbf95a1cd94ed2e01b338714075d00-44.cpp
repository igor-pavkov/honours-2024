static int perf_event_check_period(struct perf_event *event, u64 value)
{
	return event->pmu->check_period(event, value);
}