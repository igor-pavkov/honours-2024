void auth_policy_check(struct auth_request *request, const char *password,
	auth_policy_callback_t cb, void *context)
{
	if (request->master != NULL || *(request->set->policy_server_url) == '\0') {
		cb(0, context);
		return;
	}
	struct policy_lookup_ctx *ctx = p_new(request->pool, struct policy_lookup_ctx, 1);
	ctx->pool = request->pool;
	ctx->request = request;
	ctx->expect_result = TRUE;
	ctx->callback = cb;
	ctx->callback_context = context;
	ctx->set = request->set;

	auth_policy_url(ctx, "allow");
	ctx->result = (ctx->set->policy_reject_on_fail ? -1 : 0);
	auth_request_log_debug(request, "policy", "Policy request %s", ctx->url);
	T_BEGIN {
		auth_policy_create_json(ctx, password, FALSE);
	} T_END;
	auth_policy_send_request(ctx);
}