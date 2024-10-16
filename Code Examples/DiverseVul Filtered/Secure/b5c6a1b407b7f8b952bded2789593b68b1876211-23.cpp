static int parse_authenticate_response(
	git_vector *www_authenticate,
	http_subtransport *t,
	int *allowed_types)
{
	git_http_auth_context *context;
	char *challenge;
	size_t i;

	git_vector_foreach(www_authenticate, i, challenge) {
		if (auth_context_match(&context, t, challenge_match, challenge) < 0)
			return -1;
		else if (!context)
			continue;

		if (context->set_challenge &&
			context->set_challenge(context, challenge) < 0)
			return -1;

		*allowed_types |= context->credtypes;
	}

	return 0;
}