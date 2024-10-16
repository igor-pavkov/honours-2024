static u64 __perf_event_read_value(struct perf_event *event, u64 *enabled, u64 *running)
{
	struct perf_event *child;
	u64 total = 0;

	*enabled = 0;
	*running = 0;

	mutex_lock(&event->child_mutex);

	(void)perf_event_read(event, false);
	total += perf_event_count(event);

	*enabled += event->total_time_enabled +
			atomic64_read(&event->child_total_time_enabled);
	*running += event->total_time_running +
			atomic64_read(&event->child_total_time_running);

	list_for_each_entry(child, &event->child_list, child_list) {
		(void)perf_event_read(child, false);
		total += perf_event_count(child);
		*enabled += child->total_time_enabled;
		*running += child->total_time_running;
	}
	mutex_unlock(&event->child_mutex);

	return total;
}