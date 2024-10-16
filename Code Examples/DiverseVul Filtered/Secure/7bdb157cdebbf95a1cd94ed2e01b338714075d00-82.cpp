static void __perf_event_disable(struct perf_event *event,
				 struct perf_cpu_context *cpuctx,
				 struct perf_event_context *ctx,
				 void *info)
{
	if (event->state < PERF_EVENT_STATE_INACTIVE)
		return;

	if (ctx->is_active & EVENT_TIME) {
		update_context_time(ctx);
		update_cgrp_time_from_event(event);
	}

	if (event == event->group_leader)
		group_sched_out(event, cpuctx, ctx);
	else
		event_sched_out(event, cpuctx, ctx);

	perf_event_set_state(event, PERF_EVENT_STATE_OFF);
	perf_cgroup_event_disable(event, ctx);
}