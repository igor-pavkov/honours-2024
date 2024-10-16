static int merge_sched_in(struct perf_event *event, void *data)
{
	struct perf_event_context *ctx = event->ctx;
	struct perf_cpu_context *cpuctx = __get_cpu_context(ctx);
	int *can_add_hw = data;

	if (event->state <= PERF_EVENT_STATE_OFF)
		return 0;

	if (!event_filter_match(event))
		return 0;

	if (group_can_go_on(event, cpuctx, *can_add_hw)) {
		if (!group_sched_in(event, cpuctx, ctx))
			list_add_tail(&event->active_list, get_event_list(event));
	}

	if (event->state == PERF_EVENT_STATE_INACTIVE) {
		if (event->attr.pinned) {
			perf_cgroup_event_disable(event, ctx);
			perf_event_set_state(event, PERF_EVENT_STATE_ERROR);
		}

		*can_add_hw = 0;
		ctx->rotate_necessary = 1;
	}

	return 0;
}