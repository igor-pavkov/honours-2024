static void perf_event_ctx_deactivate(struct perf_event_context *ctx)
{
	lockdep_assert_irqs_disabled();

	WARN_ON(list_empty(&ctx->active_ctx_list));

	list_del_init(&ctx->active_ctx_list);
}