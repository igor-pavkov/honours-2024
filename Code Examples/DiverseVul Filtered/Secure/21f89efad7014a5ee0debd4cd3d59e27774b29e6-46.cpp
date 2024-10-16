static void enable_profile_high(gnutls_priority_t c)
{
	c->additional_verify_flags &= 0x00ffffff;
	c->additional_verify_flags |= GNUTLS_PROFILE_TO_VFLAGS(GNUTLS_PROFILE_HIGH);
	c->level = GNUTLS_SEC_PARAM_HIGH;
}