void auth_policy_deinit(void)
{
	if (http_client != NULL)
		http_client_deinit(&http_client);
	i_free(auth_policy_json_template);
}