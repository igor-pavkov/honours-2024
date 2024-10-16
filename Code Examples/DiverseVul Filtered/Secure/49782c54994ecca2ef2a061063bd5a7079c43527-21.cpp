static int _php_sasl_interact(LDAP *ld, unsigned flags, void *defaults, void *in)
{
	sasl_interact_t *interact = in;
	const char *p;
	php_ldap_bictx *ctx = defaults;

	for (;interact->id != SASL_CB_LIST_END;interact++) {
		p = NULL;
		switch(interact->id) {
			case SASL_CB_GETREALM:
				p = ctx->realm;
				break;
			case SASL_CB_AUTHNAME:
				p = ctx->authcid;
				break;
			case SASL_CB_USER:
				p = ctx->authzid;
				break;
			case SASL_CB_PASS:
				p = ctx->passwd;
				break;
		}
		if (p) {
			interact->result = p;
			interact->len = strlen(interact->result);
		}
	}
	return LDAP_SUCCESS;
}