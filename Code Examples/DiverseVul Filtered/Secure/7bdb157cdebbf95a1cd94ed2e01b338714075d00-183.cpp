static inline void perf_remove_sibling_event(struct perf_event *event)
{
	struct perf_event_context *ctx = event->ctx;
	struct perf_cpu_context *cpuctx = __get_cpu_context(ctx);

	event_sched_out(event, cpuctx, ctx);
	perf_event_set_state(event, PERF_EVENT_STATE_ERROR);
}