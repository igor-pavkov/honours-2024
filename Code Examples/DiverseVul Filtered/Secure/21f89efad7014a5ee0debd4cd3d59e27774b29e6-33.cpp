static void enable_partial_safe_renegotiation(gnutls_priority_t c)
{
	c->sr = SR_PARTIAL;
}