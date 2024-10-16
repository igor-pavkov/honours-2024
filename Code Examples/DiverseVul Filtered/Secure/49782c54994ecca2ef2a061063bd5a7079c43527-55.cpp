static php_ldap_bictx *_php_sasl_setdefs(LDAP *ld, char *sasl_mech, char *sasl_realm, char *sasl_authc_id, char *passwd, char *sasl_authz_id)
{
	php_ldap_bictx *ctx;

	ctx = ber_memalloc(sizeof(php_ldap_bictx));
	ctx->mech    = (sasl_mech) ? ber_strdup(sasl_mech) : NULL;
	ctx->realm   = (sasl_realm) ? ber_strdup(sasl_realm) : NULL;
	ctx->authcid = (sasl_authc_id) ? ber_strdup(sasl_authc_id) : NULL;
	ctx->passwd  = (passwd) ? ber_strdup(passwd) : NULL;
	ctx->authzid = (sasl_authz_id) ? ber_strdup(sasl_authz_id) : NULL;

	if (ctx->mech == NULL) {
		ldap_get_option(ld, LDAP_OPT_X_SASL_MECH, &ctx->mech);
	}
	if (ctx->realm == NULL) {
		ldap_get_option(ld, LDAP_OPT_X_SASL_REALM, &ctx->realm);
	}
	if (ctx->authcid == NULL) {
		ldap_get_option(ld, LDAP_OPT_X_SASL_AUTHCID, &ctx->authcid);
	}
	if (ctx->authzid == NULL) {
		ldap_get_option(ld, LDAP_OPT_X_SASL_AUTHZID, &ctx->authzid);
	}

	return ctx;
}