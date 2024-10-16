static void ctx_sched_out(struct perf_event_context *ctx,
			  struct perf_cpu_context *cpuctx,
			  enum event_type_t event_type)
{
	struct perf_event *event, *tmp;
	int is_active = ctx->is_active;

	lockdep_assert_held(&ctx->lock);

	if (likely(!ctx->nr_events)) {
		/*
		 * See __perf_remove_from_context().
		 */
		WARN_ON_ONCE(ctx->is_active);
		if (ctx->task)
			WARN_ON_ONCE(cpuctx->task_ctx);
		return;
	}

	ctx->is_active &= ~event_type;
	if (!(ctx->is_active & EVENT_ALL))
		ctx->is_active = 0;

	if (ctx->task) {
		WARN_ON_ONCE(cpuctx->task_ctx != ctx);
		if (!ctx->is_active)
			cpuctx->task_ctx = NULL;
	}

	/*
	 * Always update time if it was set; not only when it changes.
	 * Otherwise we can 'forget' to update time for any but the last
	 * context we sched out. For example:
	 *
	 *   ctx_sched_out(.event_type = EVENT_FLEXIBLE)
	 *   ctx_sched_out(.event_type = EVENT_PINNED)
	 *
	 * would only update time for the pinned events.
	 */
	if (is_active & EVENT_TIME) {
		/* update (and stop) ctx time */
		update_context_time(ctx);
		update_cgrp_time_from_cpuctx(cpuctx);
	}

	is_active ^= ctx->is_active; /* changed bits */

	if (!ctx->nr_active || !(is_active & EVENT_ALL))
		return;

	perf_pmu_disable(ctx->pmu);
	if (is_active & EVENT_PINNED) {
		list_for_each_entry_safe(event, tmp, &ctx->pinned_active, active_list)
			group_sched_out(event, cpuctx, ctx);
	}

	if (is_active & EVENT_FLEXIBLE) {
		list_for_each_entry_safe(event, tmp, &ctx->flexible_active, active_list)
			group_sched_out(event, cpuctx, ctx);

		/*
		 * Since we cleared EVENT_FLEXIBLE, also clear
		 * rotate_necessary, is will be reset by
		 * ctx_flexible_sched_in() when needed.
		 */
		ctx->rotate_necessary = 0;
	}
	perf_pmu_enable(ctx->pmu);
}