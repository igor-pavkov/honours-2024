int create_event_filter(struct trace_event_call *call,
			char *filter_str, bool set_str,
			struct event_filter **filterp)
{
	return create_filter(call, filter_str, set_str, filterp);
}