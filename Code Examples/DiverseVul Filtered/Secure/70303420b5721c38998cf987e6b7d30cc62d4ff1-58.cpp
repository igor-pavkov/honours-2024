void print_event_filter(struct trace_event_file *file, struct trace_seq *s)
{
	struct event_filter *filter = event_filter(file);

	if (filter && filter->filter_string)
		trace_seq_printf(s, "%s\n", filter->filter_string);
	else
		trace_seq_puts(s, "none\n");
}