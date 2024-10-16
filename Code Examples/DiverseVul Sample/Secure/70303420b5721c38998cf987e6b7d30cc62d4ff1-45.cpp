static inline struct event_filter *event_filter(struct trace_event_file *file)
{
	return file->filter;
}