static void enable_profile_suiteb128(gnutls_priority_t c)
{
	c->additional_verify_flags &= 0x00ffffff;
	c->additional_verify_flags |= GNUTLS_PROFILE_TO_VFLAGS(GNUTLS_PROFILE_SUITEB128);
	c->level = GNUTLS_SEC_PARAM_HIGH;
}