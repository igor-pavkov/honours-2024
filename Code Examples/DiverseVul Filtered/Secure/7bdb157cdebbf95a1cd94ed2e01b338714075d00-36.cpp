static void perf_event_ctx_activate(struct perf_event_context *ctx)
{
	struct list_head *head = this_cpu_ptr(&active_ctx_list);

	lockdep_assert_irqs_disabled();

	WARN_ON(!list_empty(&ctx->active_ctx_list));

	list_add(&ctx->active_ctx_list, head);
}