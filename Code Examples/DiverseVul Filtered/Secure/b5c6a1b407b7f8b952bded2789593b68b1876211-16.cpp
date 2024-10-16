static bool challenge_match(git_http_auth_scheme *scheme, void *data)
{
	const char *scheme_name = scheme->name;
	const char *challenge = (const char *)data;
	size_t scheme_len;

	scheme_len = strlen(scheme_name);
	return (strncasecmp(challenge, scheme_name, scheme_len) == 0 &&
		(challenge[scheme_len] == '\0' || challenge[scheme_len] == ' '));
}