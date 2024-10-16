static void enable_server_precedence(gnutls_priority_t c)
{
	c->server_precedence = 1;
}