const char *auth_policy_escape_function(const char *string,
	const struct auth_request *auth_request ATTR_UNUSED)
{
	string_t *tmp = t_str_new(64);
	json_append_escaped(tmp, string);
	return str_c(tmp);
}