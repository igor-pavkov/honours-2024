static void _set_lderrno(LDAP *ldap, int lderr)
{
#if !HAVE_NSLDAP
#if LDAP_API_VERSION > 2000 || HAVE_ORALDAP
	/* New versions of OpenLDAP do it this way */
	ldap_set_option(ldap, LDAP_OPT_ERROR_NUMBER, &lderr);
#else
	ldap->ld_errno = lderr;
#endif
#else
	ldap_set_lderrno(ldap, lderr, NULL, NULL);
#endif
}