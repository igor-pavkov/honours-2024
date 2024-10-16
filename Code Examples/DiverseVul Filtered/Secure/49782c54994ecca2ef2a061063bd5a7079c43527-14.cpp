PHP_FUNCTION(ldap_error)
{
	zval *link;
	ldap_linkdata *ld;
	int ld_errno;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &link) != SUCCESS) {
		return;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);

	ld_errno = _get_lderrno(ld->link);

	RETURN_STRING(ldap_err2string(ld_errno), 1);
}