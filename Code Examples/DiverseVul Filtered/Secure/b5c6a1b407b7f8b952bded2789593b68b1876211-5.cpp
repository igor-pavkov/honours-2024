static int auth_context_match(
	git_http_auth_context **out,
	http_subtransport *t,
	bool (*scheme_match)(git_http_auth_scheme *scheme, void *data),
	void *data)
{
	git_http_auth_scheme *scheme = NULL;
	git_http_auth_context *context = NULL, *c;
	size_t i;

	*out = NULL;

	for (i = 0; i < ARRAY_SIZE(auth_schemes); i++) {
		if (scheme_match(&auth_schemes[i], data)) {
			scheme = &auth_schemes[i];
			break;
		}
	}

	if (!scheme)
		return 0;

	/* See if authentication has already started for this scheme */
	git_vector_foreach(&t->auth_contexts, i, c) {
		if (c->type == scheme->type) {
			context = c;
			break;
		}
	}

	if (!context) {
		if (scheme->init_context(&context, &t->connection_data) < 0)
			return -1;
		else if (!context)
			return 0;
		else if (git_vector_insert(&t->auth_contexts, context) < 0)
			return -1;
	}

	*out = context;

	return 0;
}