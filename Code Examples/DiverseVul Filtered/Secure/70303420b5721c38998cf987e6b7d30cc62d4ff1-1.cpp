void print_subsystem_event_filter(struct event_subsystem *system,
				  struct trace_seq *s)
{
	struct event_filter *filter;

	mutex_lock(&event_mutex);
	filter = system->filter;
	if (filter && filter->filter_string)
		trace_seq_printf(s, "%s\n", filter->filter_string);
	else
		trace_seq_puts(s, DEFAULT_SYS_FILTER_MESSAGE "\n");
	mutex_unlock(&event_mutex);
}