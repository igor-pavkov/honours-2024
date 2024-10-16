static void enable_verify_allow_rsa_md5(gnutls_priority_t c)
{
	c->additional_verify_flags |=
	    GNUTLS_VERIFY_ALLOW_SIGN_RSA_MD5;
}