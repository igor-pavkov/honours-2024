static int http_close(git_smart_subtransport *subtransport)
{
	http_subtransport *t = (http_subtransport *) subtransport;
	git_http_auth_context *context;
	size_t i;

	clear_parser_state(t);

	t->connected = 0;

	if (t->io) {
		git_stream_close(t->io);
		git_stream_free(t->io);
		t->io = NULL;
	}

	if (t->cred) {
		t->cred->free(t->cred);
		t->cred = NULL;
	}

	if (t->url_cred) {
		t->url_cred->free(t->url_cred);
		t->url_cred = NULL;
	}

	git_vector_foreach(&t->auth_contexts, i, context) {
		if (context->free)
			context->free(context);
	}

	git_vector_clear(&t->auth_contexts);

	gitno_connection_data_free_ptrs(&t->connection_data);
	memset(&t->connection_data, 0x0, sizeof(gitno_connection_data));

	return 0;
}