static void remove_filter_string(struct event_filter *filter)
{
	if (!filter)
		return;

	kfree(filter->filter_string);
	filter->filter_string = NULL;
}