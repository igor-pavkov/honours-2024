group_sched_out(struct perf_event *group_event,
		struct perf_cpu_context *cpuctx,
		struct perf_event_context *ctx)
{
	struct perf_event *event;

	if (group_event->state != PERF_EVENT_STATE_ACTIVE)
		return;

	perf_pmu_disable(ctx->pmu);

	event_sched_out(group_event, cpuctx, ctx);

	/*
	 * Schedule out siblings (if any):
	 */
	for_each_sibling_event(event, group_event)
		event_sched_out(event, cpuctx, ctx);

	perf_pmu_enable(ctx->pmu);

	if (group_event->attr.exclusive)
		cpuctx->exclusive = 0;
}