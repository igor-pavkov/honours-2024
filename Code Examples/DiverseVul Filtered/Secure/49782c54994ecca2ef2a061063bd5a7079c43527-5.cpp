static void _php_sasl_freedefs(php_ldap_bictx *ctx)
{
	if (ctx->mech) ber_memfree(ctx->mech);
	if (ctx->realm) ber_memfree(ctx->realm);
	if (ctx->authcid) ber_memfree(ctx->authcid);
	if (ctx->passwd) ber_memfree(ctx->passwd);
	if (ctx->authzid) ber_memfree(ctx->authzid);
	ber_memfree(ctx);
}