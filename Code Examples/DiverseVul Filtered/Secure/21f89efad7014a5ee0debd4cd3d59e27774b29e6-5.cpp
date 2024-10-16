static void enable_safe_renegotiation(gnutls_priority_t c)
{
	c->sr = SR_SAFE;

}