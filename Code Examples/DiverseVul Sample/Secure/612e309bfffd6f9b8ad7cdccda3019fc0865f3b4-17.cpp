static int oidc_handle_request_uri(request_rec *r, oidc_cfg *c) {

	char *request_ref = NULL;
	oidc_util_get_request_parameter(r, "request_uri", &request_ref);
	if (request_ref == NULL) {
		oidc_error(r, "no \"request_uri\" parameter found");
		return HTTP_BAD_REQUEST;
	}

	const char *jwt = NULL;
	c->cache->get(r, OIDC_CACHE_SECTION_REQUEST_URI, request_ref, &jwt);
	if (jwt == NULL) {
		oidc_error(r, "no cached JWT found for request_uri reference: %s",
				request_ref);
		return HTTP_NOT_FOUND;
	}

	c->cache->set(r, OIDC_CACHE_SECTION_REQUEST_URI, request_ref, NULL, 0);

	return oidc_util_http_send(r, jwt, strlen(jwt), " application/jwt", DONE);
}