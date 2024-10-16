static void perf_event_update_time(struct perf_event *event)
{
	u64 now = perf_event_time(event);

	__perf_update_times(event, now, &event->total_time_enabled,
					&event->total_time_running);
	event->tstamp = now;
}