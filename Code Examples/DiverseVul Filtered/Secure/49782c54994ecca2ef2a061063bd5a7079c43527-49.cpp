PHP_FUNCTION(ldap_errno)
{
	zval *link;
	ldap_linkdata *ld;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &link) != SUCCESS) {
		return;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);

	RETURN_LONG(_get_lderrno(ld->link));
}