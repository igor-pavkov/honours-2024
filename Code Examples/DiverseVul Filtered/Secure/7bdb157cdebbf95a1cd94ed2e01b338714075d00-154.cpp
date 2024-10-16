static void rotate_ctx(struct perf_event_context *ctx, struct perf_event *event)
{
	/*
	 * Rotate the first entry last of non-pinned groups. Rotation might be
	 * disabled by the inheritance code.
	 */
	if (ctx->rotate_disable)
		return;

	perf_event_groups_delete(&ctx->flexible_groups, event);
	perf_event_groups_insert(&ctx->flexible_groups, event);
}