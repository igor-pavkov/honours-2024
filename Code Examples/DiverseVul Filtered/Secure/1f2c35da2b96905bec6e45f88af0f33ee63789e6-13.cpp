void auth_policy_open_key(const char *key, string_t *template)
{
	const char *ptr;
	while((ptr = strchr(key, '/')) != NULL) {
		str_append_c(template,'"');
		json_append_escaped(template, t_strndup(key, (ptr-key)));
		str_append_c(template,'"');
		str_append_c(template,':');
		str_append_c(template,'{');
		key = ptr+1;
	}
}