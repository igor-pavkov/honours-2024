static void disable_crl_checks(gnutls_priority_t c)
{
	c->additional_verify_flags |=
		GNUTLS_VERIFY_DISABLE_CRL_CHECKS;
}