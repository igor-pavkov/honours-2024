void ldap_memvfree(void **v)
{
	ldap_value_free((char **)v);
}