void auth_policy_finish(void *ctx)
{
	struct policy_lookup_ctx *context = ctx;

	if (context->parser != NULL) {
		const char *error ATTR_UNUSED;
		(void)json_parser_deinit(&(context->parser), &error);
	}
	if (context->http_request != NULL)
		http_client_request_abort(&(context->http_request));
	if (context->request != NULL)
		auth_request_unref(&context->request);
}