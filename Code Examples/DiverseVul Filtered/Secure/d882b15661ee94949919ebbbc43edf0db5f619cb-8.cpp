int gethostbyname_r(const char *name, struct hostent *ret, char *buf, size_t buflen, struct hostent **result, int *h_errnop)
{
	resolver_storage_t *storage = (void*) buf;
	int h_errno;
	test_param_t *test;

	test = get_test_param();

	*result = NULL;

	if (buflen < sizeof(*storage) + strlen(name))
		return ERANGE;

	test->tried_resolver = 1;

	if (test->plug_resolver != PLUG_NONE) {
		if (test->plug_resolver == PLUG_TIMEOUT) {
			if (test->async_mode) {
				int res;
				if ((res = write(timeout_pipe[1], "", 1)) != 1) {
					if (res == -1)
						perror("write");
					else
						fprintf(stderr, "write returned %d\n", res);
					failure();
				}
			}
			*h_errnop = TRY_AGAIN;
		} else {
			*h_errnop = HOST_NOT_FOUND;
		}
		return -1;
	}

	if ((!test->proxy_mode && strcmp(name, GG_APPMSG_HOST) != 0) || (test->proxy_mode && strcmp(name, HOST_PROXY) != 0)) {
		debug("Invalid argument for gethostbyname(): \"%s\"\n", name);
		*h_errnop = HOST_NOT_FOUND;
		return -1;
	}

	storage->addr_list[0] = (char*) &storage->addr;
	storage->addr_list[1] = NULL;
	storage->addr.s_addr = inet_addr(HOST_LOCAL);
	strcpy(storage->name, name);

	memset(ret, 0, sizeof(*ret));
	ret->h_name = storage->name;
	ret->h_addrtype = AF_INET;
	ret->h_length = sizeof(struct in_addr);
	ret->h_addr_list = storage->addr_list;
	
	*result = ret;

	return 0;
}