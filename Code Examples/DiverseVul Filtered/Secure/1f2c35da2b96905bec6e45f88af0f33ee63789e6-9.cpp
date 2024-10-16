void auth_policy_url(struct policy_lookup_ctx *context, const char *command)
{
	size_t len = strlen(context->set->policy_server_url);
	if (context->set->policy_server_url[len-1] == '&')
		context->url = p_strdup_printf(context->pool, "%scommand=%s",
			context->set->policy_server_url, command);
	else
		context->url = p_strdup_printf(context->pool, "%s?command=%s",
			context->set->policy_server_url, command);
}