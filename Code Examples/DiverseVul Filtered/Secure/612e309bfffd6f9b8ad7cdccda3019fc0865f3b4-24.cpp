static int oidc_handle_remove_at_cache(request_rec *r, oidc_cfg *c) {
	char *access_token = NULL;
	oidc_util_get_request_parameter(r, "remove_at_cache", &access_token);

	const char *cache_entry = NULL;
	c->cache->get(r, OIDC_CACHE_SECTION_ACCESS_TOKEN, access_token, &cache_entry);
	if (cache_entry == NULL) {
		oidc_error(r, "no cached access token found for value: %s", access_token);
		return HTTP_NOT_FOUND;
	}

	c->cache->set(r, OIDC_CACHE_SECTION_ACCESS_TOKEN, access_token, NULL, 0);

	return DONE;
}