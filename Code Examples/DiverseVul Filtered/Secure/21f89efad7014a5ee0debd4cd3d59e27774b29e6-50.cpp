static void disable_safe_renegotiation(gnutls_priority_t c)
{
	c->sr = SR_DISABLED;
}