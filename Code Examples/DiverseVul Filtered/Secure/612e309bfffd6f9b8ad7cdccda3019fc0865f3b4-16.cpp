static int oidc_handle_session_management_iframe_op(request_rec *r, oidc_cfg *c,
		oidc_session_t *session, const char *check_session_iframe) {
	oidc_debug(r, "enter");
	apr_table_add(r->headers_out, "Location", check_session_iframe);
	return HTTP_MOVED_TEMPORARILY;
}