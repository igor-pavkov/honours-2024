void auth_policy_process_response(const struct http_response *response,
	void *ctx)
{
	struct policy_lookup_ctx *context = ctx;

	context->payload = response->payload;

	if ((response->status / 10) != 20) {
		auth_request_log_error(context->request, "policy",
			"Policy server HTTP error: %d %s", response->status, response->reason);
		if (context->callback != NULL)
			context->callback(context->result, context->callback_context);
		return;
	}

	if (response->payload == NULL) {
		if (context->expect_result)
			auth_request_log_error(context->request, "policy",
				"Policy server result was empty");
		if (context->callback != NULL)
			context->callback(context->result, context->callback_context);
		return;
	}

	if (context->expect_result) {
		i_stream_ref(response->payload);
		context->io = io_add_istream(response->payload, auth_policy_parse_response, context);
		context->parser = json_parser_init(response->payload);
		auth_policy_parse_response(ctx);
	} else {
		auth_request_log_debug(context->request, "policy",
			"Policy response %d", context->result);
		if (context->callback != NULL)
			context->callback(context->result, context->callback_context);
	}
}