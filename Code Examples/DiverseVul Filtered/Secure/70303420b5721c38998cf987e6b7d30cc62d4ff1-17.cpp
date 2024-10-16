static inline void event_set_filter(struct trace_event_file *file,
				    struct event_filter *filter)
{
	rcu_assign_pointer(file->filter, filter);
}