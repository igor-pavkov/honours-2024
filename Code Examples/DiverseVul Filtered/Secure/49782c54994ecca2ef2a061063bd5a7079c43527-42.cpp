LDAPControl *ldap_control_find( const char *oid, LDAPControl **ctrls, LDAPControl ***nextctrlp)
{
	assert(nextctrlp == NULL);
	return ldap_find_control(oid, ctrls);
}