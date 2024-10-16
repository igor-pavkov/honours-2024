static int perf_event_bpf_match(struct perf_event *event)
{
	return event->attr.bpf_event;
}