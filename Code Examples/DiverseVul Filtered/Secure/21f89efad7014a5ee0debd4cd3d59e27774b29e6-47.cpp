static void enable_fallback_scsv(gnutls_priority_t c)
{
	c->fallback = 1;
}