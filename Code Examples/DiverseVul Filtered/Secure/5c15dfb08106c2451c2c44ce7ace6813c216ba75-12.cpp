authz_status oidc_authz_checker_claims_expr(request_rec *r, const char *require_args, const void *parsed_require_args) {
	return oidc_authz_checker(r, require_args, parsed_require_args, oidc_authz_match_claims_expr);
}