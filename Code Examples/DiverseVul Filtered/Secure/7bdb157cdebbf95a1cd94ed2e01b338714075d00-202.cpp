static int perf_event_namespaces_match(struct perf_event *event)
{
	return event->attr.namespaces;
}