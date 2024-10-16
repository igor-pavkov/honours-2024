void perf_event_free_task(struct task_struct *task)
{
	struct perf_event_context *ctx;
	struct perf_event *event, *tmp;
	int ctxn;

	for_each_task_context_nr(ctxn) {
		ctx = task->perf_event_ctxp[ctxn];
		if (!ctx)
			continue;

		mutex_lock(&ctx->mutex);
		raw_spin_lock_irq(&ctx->lock);
		/*
		 * Destroy the task <-> ctx relation and mark the context dead.
		 *
		 * This is important because even though the task hasn't been
		 * exposed yet the context has been (through child_list).
		 */
		RCU_INIT_POINTER(task->perf_event_ctxp[ctxn], NULL);
		WRITE_ONCE(ctx->task, TASK_TOMBSTONE);
		put_task_struct(task); /* cannot be last */
		raw_spin_unlock_irq(&ctx->lock);

		list_for_each_entry_safe(event, tmp, &ctx->event_list, event_entry)
			perf_free_event(event, ctx);

		mutex_unlock(&ctx->mutex);

		/*
		 * perf_event_release_kernel() could've stolen some of our
		 * child events and still have them on its free_list. In that
		 * case we must wait for these events to have been freed (in
		 * particular all their references to this task must've been
		 * dropped).
		 *
		 * Without this copy_process() will unconditionally free this
		 * task (irrespective of its reference count) and
		 * _free_event()'s put_task_struct(event->hw.target) will be a
		 * use-after-free.
		 *
		 * Wait for all events to drop their context reference.
		 */
		wait_var_event(&ctx->refcount, refcount_read(&ctx->refcount) == 1);
		put_ctx(ctx); /* must be last */
	}
}