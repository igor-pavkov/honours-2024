struct sys_notify_context *sys_notify_context_create(TALLOC_CTX *mem_ctx,
						     struct tevent_context *ev)
{
	struct sys_notify_context *ctx;

	if (!(ctx = talloc(mem_ctx, struct sys_notify_context))) {
		DEBUG(0, ("talloc failed\n"));
		return NULL;
	}

	ctx->ev = ev;
	ctx->private_data = NULL;
	return ctx;
}