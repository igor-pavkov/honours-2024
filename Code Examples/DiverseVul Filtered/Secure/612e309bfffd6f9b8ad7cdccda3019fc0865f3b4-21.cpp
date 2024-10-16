static int oidc_handle_logout_request(request_rec *r, oidc_cfg *c,
		oidc_session_t *session, const char *url) {

	oidc_debug(r, "enter (url=%s)", url);

	/* if there's no remote_user then there's no (stored) session to kill */
	if (session->remote_user != NULL) {

		/* remove session state (cq. cache entry and cookie) */
		oidc_session_kill(r, session);
	}

	/* see if this is the OP calling us */
	if (oidc_is_front_channel_logout(url)) {

		/* set recommended cache control headers */
		apr_table_add(r->err_headers_out, "Cache-Control",
				"no-cache, no-store");
		apr_table_add(r->err_headers_out, "Pragma", "no-cache");
		apr_table_add(r->err_headers_out, "P3P", "CAO PSA OUR");
		apr_table_add(r->err_headers_out, "Expires", "0");
		apr_table_add(r->err_headers_out, "X-Frame-Options", "DENY");

		/* see if this is PF-PA style logout in which case we return a transparent pixel */
		const char *accept = apr_table_get(r->headers_in, "Accept");
		if ((apr_strnatcmp(url, OIDC_IMG_STYLE_LOGOUT_PARAM_VALUE) == 0)
				|| ((accept) && strstr(accept, "image/png"))) {
			return oidc_util_http_send(r,
					(const char *) &oidc_transparent_pixel,
					sizeof(oidc_transparent_pixel), "image/png", DONE);
		}

		/* standard HTTP based logout: should be called in an iframe from the OP */
		return oidc_util_html_send(r, "Logged Out", NULL, NULL,
				"<p>Logged Out</p>", DONE);
	}

	/* see if we don't need to go somewhere special after killing the session locally */
	if (url == NULL)
		return oidc_util_html_send(r, "Logged Out", NULL, NULL,
				"<p>Logged Out</p>", DONE);

	/* send the user to the specified where-to-go-after-logout URL */
	apr_table_add(r->headers_out, "Location", url);

	return HTTP_MOVED_TEMPORARILY;
}