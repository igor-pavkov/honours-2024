static inline void __free_subsystem_filter(struct trace_event_file *file)
{
	__free_filter(file->filter);
	file->filter = NULL;
}