PHP_FUNCTION(ldap_dn2ufn)
{
	char *dn, *ufn;
	int dn_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &dn, &dn_len) != SUCCESS) {
		return;
	}

	ufn = ldap_dn2ufn(dn);

	if (ufn != NULL) {
		RETVAL_STRING(ufn, 1);
#if (LDAP_API_VERSION > 2000) || HAVE_NSLDAP || HAVE_ORALDAP || WINDOWS
		ldap_memfree(ufn);
#endif
	} else {
		RETURN_FALSE;
	}
}