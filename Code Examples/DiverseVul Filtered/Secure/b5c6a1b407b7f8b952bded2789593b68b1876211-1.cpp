static int on_header_ready(http_subtransport *t)
{
	git_buf *name = &t->parse_header_name;
	git_buf *value = &t->parse_header_value;

	if (!strcasecmp("Content-Type", git_buf_cstr(name))) {
		if (!t->content_type) {
			t->content_type = git__strdup(git_buf_cstr(value));
			GITERR_CHECK_ALLOC(t->content_type);
		}
	}
	else if (!strcasecmp("WWW-Authenticate", git_buf_cstr(name))) {
		char *dup = git__strdup(git_buf_cstr(value));
		GITERR_CHECK_ALLOC(dup);

		git_vector_insert(&t->www_authenticate, dup);
	}
	else if (!strcasecmp("Location", git_buf_cstr(name))) {
		if (!t->location) {
			t->location = git__strdup(git_buf_cstr(value));
			GITERR_CHECK_ALLOC(t->location);
		}
	}

	return 0;
}