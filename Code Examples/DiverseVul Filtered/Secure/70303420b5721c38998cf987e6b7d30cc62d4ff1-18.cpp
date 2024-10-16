static inline void __remove_filter(struct trace_event_file *file)
{
	filter_disable(file);
	remove_filter_string(file->filter);
}