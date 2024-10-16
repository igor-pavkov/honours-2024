PHP_METHOD(snmp, close)
{
	php_snmp_object *snmp_object;
	zval *object = getThis();

	snmp_object = Z_SNMP_P(object);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_FALSE;
	}

	netsnmp_session_free(&(snmp_object->session));

	RETURN_TRUE;
}