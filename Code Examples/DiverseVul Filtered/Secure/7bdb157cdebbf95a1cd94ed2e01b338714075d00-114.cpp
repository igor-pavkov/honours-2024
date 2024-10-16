u64 perf_event_read_value(struct perf_event *event, u64 *enabled, u64 *running)
{
	struct perf_event_context *ctx;
	u64 count;

	ctx = perf_event_ctx_lock(event);
	count = __perf_event_read_value(event, enabled, running);
	perf_event_ctx_unlock(event, ctx);

	return count;
}