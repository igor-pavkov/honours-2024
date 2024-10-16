static int on_headers_complete(http_parser *parser)
{
	parser_context *ctx = (parser_context *) parser->data;
	http_subtransport *t = ctx->t;
	http_stream *s = ctx->s;
	git_buf buf = GIT_BUF_INIT;
	int error = 0, no_callback = 0, allowed_auth_types = 0;

	/* Both parse_header_name and parse_header_value are populated
	 * and ready for consumption. */
	if (VALUE == t->last_cb)
		if (on_header_ready(t) < 0)
			return t->parse_error = PARSE_ERROR_GENERIC;

	/* Capture authentication headers which may be a 401 (authentication
	 * is not complete) or a 200 (simply informing us that auth *is*
	 * complete.)
	 */
	if (parse_authenticate_response(&t->www_authenticate, t,
			&allowed_auth_types) < 0)
		return t->parse_error = PARSE_ERROR_GENERIC;

	/* Check for an authentication failure. */
	if (parser->status_code == 401 && get_verb == s->verb) {
		if (!t->owner->cred_acquire_cb) {
			no_callback = 1;
		} else {
			if (allowed_auth_types) {
				if (t->cred) {
					t->cred->free(t->cred);
					t->cred = NULL;
				}

				error = t->owner->cred_acquire_cb(&t->cred,
								  t->owner->url,
								  t->connection_data.user,
								  allowed_auth_types,
								  t->owner->cred_acquire_payload);

				if (error == GIT_PASSTHROUGH) {
					no_callback = 1;
				} else if (error < 0) {
					t->error = error;
					return t->parse_error = PARSE_ERROR_EXT;
				} else {
					assert(t->cred);

					if (!(t->cred->credtype & allowed_auth_types)) {
						giterr_set(GITERR_NET, "credentials callback returned an invalid cred type");
						return t->parse_error = PARSE_ERROR_GENERIC;
					}

					/* Successfully acquired a credential. */
					t->parse_error = PARSE_ERROR_REPLAY;
					return 0;
				}
			}
		}

		if (no_callback) {
			giterr_set(GITERR_NET, "authentication required but no callback set");
			return t->parse_error = PARSE_ERROR_GENERIC;
		}
	}

	/* Check for a redirect.
	 * Right now we only permit a redirect to the same hostname. */
	if ((parser->status_code == 301 ||
	     parser->status_code == 302 ||
	     (parser->status_code == 303 && get_verb == s->verb) ||
	     parser->status_code == 307) &&
	    t->location) {

		if (s->redirect_count >= 7) {
			giterr_set(GITERR_NET, "Too many redirects");
			return t->parse_error = PARSE_ERROR_GENERIC;
		}

		if (gitno_connection_data_from_url(&t->connection_data, t->location, s->service_url) < 0)
			return t->parse_error = PARSE_ERROR_GENERIC;

		/* Set the redirect URL on the stream. This is a transfer of
		 * ownership of the memory. */
		if (s->redirect_url)
			git__free(s->redirect_url);

		s->redirect_url = t->location;
		t->location = NULL;

		t->connected = 0;
		s->redirect_count++;

		t->parse_error = PARSE_ERROR_REPLAY;
		return 0;
	}

	/* Check for a 200 HTTP status code. */
	if (parser->status_code != 200) {
		giterr_set(GITERR_NET,
			"Unexpected HTTP status code: %d",
			parser->status_code);
		return t->parse_error = PARSE_ERROR_GENERIC;
	}

	/* The response must contain a Content-Type header. */
	if (!t->content_type) {
		giterr_set(GITERR_NET, "No Content-Type header in response");
		return t->parse_error = PARSE_ERROR_GENERIC;
	}

	/* The Content-Type header must match our expectation. */
	if (get_verb == s->verb)
		git_buf_printf(&buf,
			"application/x-git-%s-advertisement",
			ctx->s->service);
	else
		git_buf_printf(&buf,
			"application/x-git-%s-result",
			ctx->s->service);

	if (git_buf_oom(&buf))
		return t->parse_error = PARSE_ERROR_GENERIC;

	if (strcmp(t->content_type, git_buf_cstr(&buf))) {
		git_buf_free(&buf);
		giterr_set(GITERR_NET,
			"Invalid Content-Type: %s",
			t->content_type);
		return t->parse_error = PARSE_ERROR_GENERIC;
	}

	git_buf_free(&buf);

	return 0;
}