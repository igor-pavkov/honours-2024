PHP_FUNCTION(ldap_err2str)
{
	long perrno;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &perrno) != SUCCESS) {
		return;
	}

	RETURN_STRING(ldap_err2string(perrno), 1);
}