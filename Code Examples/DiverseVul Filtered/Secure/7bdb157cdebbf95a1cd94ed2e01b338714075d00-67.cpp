static void perf_event_free_bpf_prog(struct perf_event *event)
{
	if (!perf_event_is_tracing(event)) {
		perf_event_free_bpf_handler(event);
		return;
	}
	perf_event_detach_bpf_prog(event);
}