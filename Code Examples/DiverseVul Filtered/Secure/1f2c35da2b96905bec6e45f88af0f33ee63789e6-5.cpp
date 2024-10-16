void auth_policy_send_request(struct policy_lookup_ctx *context)
{
	const char *error;
	struct http_url *url;
	if (http_url_parse(context->url, NULL, HTTP_URL_ALLOW_USERINFO_PART,
			   context->pool, &url, &error) != 0) {
		auth_request_log_error(context->request, "policy",
			"Could not parse url %s: %s", context->url, error);
		auth_policy_finish(context);
		return;
	}
	context->http_request = http_client_request_url(http_client,
		"POST", url, auth_policy_process_response, (void*)context);
	http_client_request_set_destroy_callback(context->http_request, auth_policy_finish, context);
	http_client_request_add_header(context->http_request, "Content-Type", "application/json");
	if (*context->set->policy_server_api_header != 0) {
		const char *ptr;
		if ((ptr = strstr(context->set->policy_server_api_header, ":")) != NULL) {
			const char *header = t_strcut(context->set->policy_server_api_header, ':');
			http_client_request_add_header(context->http_request, header, ptr + 1);
		} else {
			http_client_request_add_header(context->http_request,
				"X-API-Key", context->set->policy_server_api_header);
		}
	}
	if (url->user != NULL) {
		/* allow empty password */
		http_client_request_set_auth_simple(context->http_request, url->user,
			(url->password != NULL ? url->password : ""));
	}
	struct istream *is = i_stream_create_from_buffer(context->json);
	http_client_request_set_payload(context->http_request, is, FALSE);
	i_stream_unref(&is);
	http_client_request_submit(context->http_request);
	auth_request_ref(context->request);
}