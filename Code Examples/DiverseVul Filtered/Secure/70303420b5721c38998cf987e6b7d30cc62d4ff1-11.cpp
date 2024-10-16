static void filter_free_subsystem_preds(struct trace_subsystem_dir *dir,
					struct trace_array *tr)
{
	struct trace_event_file *file;

	list_for_each_entry(file, &tr->events, list) {
		if (file->system != dir)
			continue;
		__remove_filter(file);
	}
}