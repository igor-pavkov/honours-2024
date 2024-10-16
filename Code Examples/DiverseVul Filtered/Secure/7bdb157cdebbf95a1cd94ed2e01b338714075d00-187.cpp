static inline struct list_head *get_event_list(struct perf_event *event)
{
	struct perf_event_context *ctx = event->ctx;
	return event->attr.pinned ? &ctx->pinned_active : &ctx->flexible_active;
}