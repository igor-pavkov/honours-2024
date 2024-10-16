static void perf_event_context_sched_in(struct perf_event_context *ctx,
					struct task_struct *task)
{
	struct perf_cpu_context *cpuctx;
	struct pmu *pmu = ctx->pmu;

	cpuctx = __get_cpu_context(ctx);
	if (cpuctx->task_ctx == ctx) {
		if (cpuctx->sched_cb_usage)
			__perf_pmu_sched_task(cpuctx, true);
		return;
	}

	perf_ctx_lock(cpuctx, ctx);
	/*
	 * We must check ctx->nr_events while holding ctx->lock, such
	 * that we serialize against perf_install_in_context().
	 */
	if (!ctx->nr_events)
		goto unlock;

	perf_pmu_disable(pmu);
	/*
	 * We want to keep the following priority order:
	 * cpu pinned (that don't need to move), task pinned,
	 * cpu flexible, task flexible.
	 *
	 * However, if task's ctx is not carrying any pinned
	 * events, no need to flip the cpuctx's events around.
	 */
	if (!RB_EMPTY_ROOT(&ctx->pinned_groups.tree))
		cpu_ctx_sched_out(cpuctx, EVENT_FLEXIBLE);
	perf_event_sched_in(cpuctx, ctx, task);

	if (cpuctx->sched_cb_usage && pmu->sched_task)
		pmu->sched_task(cpuctx->task_ctx, true);

	perf_pmu_enable(pmu);

unlock:
	perf_ctx_unlock(cpuctx, ctx);
}