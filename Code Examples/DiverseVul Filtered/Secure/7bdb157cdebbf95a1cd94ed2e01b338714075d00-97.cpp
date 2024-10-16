static int event_function(void *info)
{
	struct event_function_struct *efs = info;
	struct perf_event *event = efs->event;
	struct perf_event_context *ctx = event->ctx;
	struct perf_cpu_context *cpuctx = __get_cpu_context(ctx);
	struct perf_event_context *task_ctx = cpuctx->task_ctx;
	int ret = 0;

	lockdep_assert_irqs_disabled();

	perf_ctx_lock(cpuctx, task_ctx);
	/*
	 * Since we do the IPI call without holding ctx->lock things can have
	 * changed, double check we hit the task we set out to hit.
	 */
	if (ctx->task) {
		if (ctx->task != current) {
			ret = -ESRCH;
			goto unlock;
		}

		/*
		 * We only use event_function_call() on established contexts,
		 * and event_function() is only ever called when active (or
		 * rather, we'll have bailed in task_function_call() or the
		 * above ctx->task != current test), therefore we must have
		 * ctx->is_active here.
		 */
		WARN_ON_ONCE(!ctx->is_active);
		/*
		 * And since we have ctx->is_active, cpuctx->task_ctx must
		 * match.
		 */
		WARN_ON_ONCE(task_ctx != ctx);
	} else {
		WARN_ON_ONCE(&cpuctx->ctx != ctx);
	}

	efs->func(event, cpuctx, ctx, efs->data);
unlock:
	perf_ctx_unlock(cpuctx, task_ctx);

	return ret;
}