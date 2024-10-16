static void disable_wildcards(gnutls_priority_t c)
{
	c->additional_verify_flags |= GNUTLS_VERIFY_DO_NOT_ALLOW_WILDCARDS;
}