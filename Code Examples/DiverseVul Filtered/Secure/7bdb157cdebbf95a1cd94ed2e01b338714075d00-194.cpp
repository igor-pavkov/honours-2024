static void perf_event_groups_init(struct perf_event_groups *groups)
{
	groups->tree = RB_ROOT;
	groups->index = 0;
}