static void enable_profile_very_weak(gnutls_priority_t c)
{
	c->additional_verify_flags &= 0x00ffffff;
	c->additional_verify_flags |= GNUTLS_PROFILE_TO_VFLAGS(GNUTLS_PROFILE_VERY_WEAK);
	c->level = GNUTLS_SEC_PARAM_VERY_WEAK;
}