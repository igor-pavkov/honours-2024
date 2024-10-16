static void filter_free_subsystem_filters(struct trace_subsystem_dir *dir,
					  struct trace_array *tr)
{
	struct trace_event_file *file;

	list_for_each_entry(file, &tr->events, list) {
		if (file->system != dir)
			continue;
		__free_subsystem_filter(file);
	}
}