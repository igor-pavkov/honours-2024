static void enable_latest_record_version(gnutls_priority_t c)
{
	c->min_record_version = 0;
}