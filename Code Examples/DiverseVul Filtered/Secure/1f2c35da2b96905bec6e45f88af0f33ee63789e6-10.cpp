const struct var_expand_table *policy_get_var_expand_table(struct auth_request *auth_request,
	const char *hashed_password)
{
	struct var_expand_table *table;
	unsigned int count = 1;

	table = auth_request_get_var_expand_table_full(auth_request, auth_policy_escape_function,
						       &count);
	table[0].key = '\0';
	table[0].long_key = "hashed_password";
	table[0].value = hashed_password;
	if (table[0].value != NULL)
		table[0].value = auth_policy_escape_function(table[0].value, auth_request);

	return table;
}