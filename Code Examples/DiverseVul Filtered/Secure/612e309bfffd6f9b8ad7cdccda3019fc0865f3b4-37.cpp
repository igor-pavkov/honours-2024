static apr_byte_t oidc_provider_static_config(request_rec *r, oidc_cfg *c,
		oidc_provider_t **provider) {

	json_t *j_provider = NULL;
	const char *s_json = NULL;

	/* see if we should configure a static provider based on external (cached) metadata */
	if ((c->metadata_dir != NULL) || (c->provider.metadata_url == NULL)) {
		*provider = &c->provider;
		return TRUE;
	}

	c->cache->get(r, OIDC_CACHE_SECTION_PROVIDER,
			oidc_util_escape_string(r, c->provider.metadata_url), &s_json);

	if (s_json == NULL) {

		if (oidc_metadata_provider_retrieve(r, c, NULL,
				c->provider.metadata_url, &j_provider, &s_json) == FALSE) {
			oidc_error(r, "could not retrieve metadata from url: %s",
					c->provider.metadata_url);
			return FALSE;
		}

		c->cache->set(r, OIDC_CACHE_SECTION_PROVIDER,
				oidc_util_escape_string(r, c->provider.metadata_url), s_json,
				apr_time_now()
				+ (c->provider_metadata_refresh_interval <= 0 ?
						apr_time_from_sec(
								OIDC_CACHE_PROVIDER_METADATA_EXPIRY_DEFAULT) :
								c->provider_metadata_refresh_interval));

	} else {

		/* correct parsing and validation was already done when it was put in the cache */
		j_provider = json_loads(s_json, 0, 0);
	}

	*provider = apr_pcalloc(r->pool, sizeof(oidc_provider_t));
	memcpy(*provider, &c->provider, sizeof(oidc_provider_t));

	if (oidc_metadata_provider_parse(r, c, j_provider, *provider) == FALSE) {
		oidc_error(r, "could not parse metadata from url: %s",
				c->provider.metadata_url);
		if (j_provider)
			json_decref(j_provider);
		return FALSE;
	}

	json_decref(j_provider);

	return TRUE;
}