void ftrace_profile_free_filter(struct perf_event *event)
{
	struct event_filter *filter = event->filter;

	event->filter = NULL;
	__free_filter(filter);
}