static void get_ctx(struct perf_event_context *ctx)
{
	refcount_inc(&ctx->refcount);
}