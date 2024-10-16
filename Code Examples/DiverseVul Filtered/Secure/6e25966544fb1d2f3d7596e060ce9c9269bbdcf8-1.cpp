PHP_METHOD(snmp, setSecurity)
{
	php_snmp_object *snmp_object;
	zval *object = getThis();
	char *a1 = "", *a2 = "", *a3 = "", *a4 = "", *a5 = "", *a6 = "", *a7 = "";
	size_t a1_len = 0, a2_len = 0, a3_len = 0, a4_len = 0, a5_len = 0, a6_len = 0, a7_len = 0;
	int argc = ZEND_NUM_ARGS();

	snmp_object = Z_SNMP_P(object);

	if (zend_parse_parameters(argc, "s|ssssss", &a1, &a1_len, &a2, &a2_len, &a3, &a3_len,
		&a4, &a4_len, &a5, &a5_len, &a6, &a6_len, &a7, &a7_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (netsnmp_session_set_security(snmp_object->session, a1, a2, a3, a4, a5, a6, a7)) {
		/* Warning message sent already, just bail out */
		RETURN_FALSE;
	}
	RETURN_TRUE;
}