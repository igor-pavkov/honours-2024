static void enable_profile_ultra(gnutls_priority_t c)
{
	c->additional_verify_flags &= 0x00ffffff;
	c->additional_verify_flags |= GNUTLS_PROFILE_TO_VFLAGS(GNUTLS_PROFILE_ULTRA);
	c->level = GNUTLS_SEC_PARAM_ULTRA;
}