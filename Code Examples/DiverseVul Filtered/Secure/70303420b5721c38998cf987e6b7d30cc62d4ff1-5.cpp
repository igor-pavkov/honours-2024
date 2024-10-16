static void __free_filter(struct event_filter *filter)
{
	if (!filter)
		return;

	free_prog(filter);
	kfree(filter->filter_string);
	kfree(filter);
}