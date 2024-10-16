static json_t * oidc_get_state_from_cookie(request_rec *r, oidc_cfg *c,
		const char *cookieValue) {
	json_t *result = NULL;
	oidc_util_jwt_verify(r, c->crypto_passphrase, cookieValue, &result);
	return result;
}