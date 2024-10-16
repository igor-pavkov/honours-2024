static int _get_lderrno(LDAP *ldap)
{
#if !HAVE_NSLDAP
#if LDAP_API_VERSION > 2000 || HAVE_ORALDAP
	int lderr;

	/* New versions of OpenLDAP do it this way */
	ldap_get_option(ldap, LDAP_OPT_ERROR_NUMBER, &lderr);
	return lderr;
#else
	return ldap->ld_errno;
#endif
#else
	return ldap_get_lderrno(ldap, NULL, NULL);
#endif
}