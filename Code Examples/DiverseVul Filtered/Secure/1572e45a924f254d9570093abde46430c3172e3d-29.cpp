static enum event_type_t get_event_type(struct perf_event *event)
{
	struct perf_event_context *ctx = event->ctx;
	enum event_type_t event_type;

	lockdep_assert_held(&ctx->lock);

	event_type = event->attr.pinned ? EVENT_PINNED : EVENT_FLEXIBLE;
	if (!ctx->task)
		event_type |= EVENT_CPU;

	return event_type;
}