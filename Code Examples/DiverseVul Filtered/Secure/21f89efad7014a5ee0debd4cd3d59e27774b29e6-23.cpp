static void enable_stateless_compression(gnutls_priority_t c)
{
	c->stateless_compression = 1;
}