static void add_event_to_ctx(struct perf_event *event,
			       struct perf_event_context *ctx)
{
	list_add_event(event, ctx);
	perf_group_attach(event);
}