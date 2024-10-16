static enum event_type_t get_event_type(struct perf_event *event)
{
	struct perf_event_context *ctx = event->ctx;
	enum event_type_t event_type;

	lockdep_assert_held(&ctx->lock);

	/*
	 * It's 'group type', really, because if our group leader is
	 * pinned, so are we.
	 */
	if (event->group_leader != event)
		event = event->group_leader;

	event_type = event->attr.pinned ? EVENT_PINNED : EVENT_FLEXIBLE;
	if (!ctx->task)
		event_type |= EVENT_CPU;

	return event_type;
}