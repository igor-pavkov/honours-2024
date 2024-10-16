void auth_policy_close_key(const char *key, string_t *template)
{
	while((key = strchr(key, '/')) != NULL) { str_append_c(template,'}'); key++; }
}