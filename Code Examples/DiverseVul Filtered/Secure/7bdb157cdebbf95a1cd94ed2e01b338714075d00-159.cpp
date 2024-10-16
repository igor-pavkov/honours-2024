int perf_event_period(struct perf_event *event, u64 value)
{
	struct perf_event_context *ctx;
	int ret;

	ctx = perf_event_ctx_lock(event);
	ret = _perf_event_period(event, value);
	perf_event_ctx_unlock(event, ctx);

	return ret;
}