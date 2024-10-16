static inline void event_clear_filter(struct trace_event_file *file)
{
	RCU_INIT_POINTER(file->filter, NULL);
}