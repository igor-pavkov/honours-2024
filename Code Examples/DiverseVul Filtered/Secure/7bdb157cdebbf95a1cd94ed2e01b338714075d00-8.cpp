static void perf_event_update_sibling_time(struct perf_event *leader)
{
	struct perf_event *sibling;

	for_each_sibling_event(sibling, leader)
		perf_event_update_time(sibling);
}