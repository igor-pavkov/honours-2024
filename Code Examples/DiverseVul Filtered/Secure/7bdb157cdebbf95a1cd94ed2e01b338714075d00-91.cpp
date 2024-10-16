static void init_event_group(struct perf_event *event)
{
	RB_CLEAR_NODE(&event->group_node);
	event->group_index = 0;
}