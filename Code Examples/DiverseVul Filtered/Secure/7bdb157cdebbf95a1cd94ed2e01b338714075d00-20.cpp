static int perf_event_cgroup_match(struct perf_event *event)
{
	return event->attr.cgroup;
}