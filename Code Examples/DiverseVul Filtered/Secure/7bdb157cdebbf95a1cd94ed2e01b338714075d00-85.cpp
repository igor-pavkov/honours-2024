static void free_ctx(struct rcu_head *head)
{
	struct perf_event_context *ctx;

	ctx = container_of(head, struct perf_event_context, rcu_head);
	free_task_ctx_data(ctx->pmu, ctx->task_ctx_data);
	kfree(ctx);
}