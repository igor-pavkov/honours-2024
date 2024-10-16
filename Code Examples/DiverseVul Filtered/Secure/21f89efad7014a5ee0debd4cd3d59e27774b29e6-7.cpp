static void enable_no_ext_master_secret(gnutls_priority_t c)
{
	c->no_ext_master_secret = 1;
}