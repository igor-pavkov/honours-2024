u64 perf_event_pause(struct perf_event *event, bool reset)
{
	struct perf_event_context *ctx;
	u64 count;

	ctx = perf_event_ctx_lock(event);
	WARN_ON_ONCE(event->attr.inherit);
	_perf_event_disable(event);
	count = local64_read(&event->count);
	if (reset)
		local64_set(&event->count, 0);
	perf_event_ctx_unlock(event, ctx);

	return count;
}