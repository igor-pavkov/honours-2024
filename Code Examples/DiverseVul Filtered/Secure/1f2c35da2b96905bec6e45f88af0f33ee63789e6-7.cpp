void auth_policy_report(struct auth_request *request)
{
	if (request->master != NULL)
		return;

	if (*(request->set->policy_server_url) == '\0')
		return;
	struct policy_lookup_ctx *ctx = p_new(request->pool, struct policy_lookup_ctx, 1);
	ctx->pool = request->pool;
	ctx->request = request;
	ctx->expect_result = FALSE;
	ctx->set = request->set;
	auth_policy_url(ctx, "report");
	auth_request_log_debug(request, "policy", "Policy request %s", ctx->url);
	T_BEGIN {
		auth_policy_create_json(ctx, request->mech_password, TRUE);
	} T_END;
	auth_policy_send_request(ctx);
}