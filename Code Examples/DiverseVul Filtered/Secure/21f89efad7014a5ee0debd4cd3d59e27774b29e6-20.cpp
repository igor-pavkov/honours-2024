static void enable_ssl3_record_version(gnutls_priority_t c)
{
	c->min_record_version = 1;
}