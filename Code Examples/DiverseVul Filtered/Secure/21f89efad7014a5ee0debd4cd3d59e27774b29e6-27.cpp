static void enable_unsafe_renegotiation(gnutls_priority_t c)
{
	c->sr = SR_UNSAFE;
}