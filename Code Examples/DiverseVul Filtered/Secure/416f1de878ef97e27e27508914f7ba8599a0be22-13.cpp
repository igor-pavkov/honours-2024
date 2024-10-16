void set_peername_from_env(address_t *p, const char *env)
{
	struct addrinfo hints = {
		.ai_family = AF_UNSPEC,
		.ai_socktype = SOCK_STREAM,
		.ai_flags = AI_NUMERICHOST | AI_NUMERICSERV,
	};
	struct addrinfo *result;
	char *c;
	int s;

	char *val = getenv(env);
	csync_debug(3, "getenv(%s): >>%s<<\n", env, val ?: "");
	if (!val)
		return;
	val = strdup(val);
	if (!val)
		return;

	c = strchr(val, ' ');
	if (!c)
		return;
	*c = '\0';

	s = getaddrinfo(val, NULL, &hints, &result);
	if (s != 0) {
		csync_debug(1, "getaddrinfo: %s\n", gai_strerror(s));
		return;
	}

	/* getaddrinfo() may return a list of address structures.
	 * Use the first one. */
	if (result)
		memcpy(p, result->ai_addr, result->ai_addrlen);
	freeaddrinfo(result);
}