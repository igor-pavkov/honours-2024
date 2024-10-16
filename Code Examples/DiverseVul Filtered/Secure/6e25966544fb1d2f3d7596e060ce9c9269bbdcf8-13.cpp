static int php_snmp_read_info(php_snmp_object *snmp_object, zval *retval)
{
	zval val;

	array_init(retval);

	if (snmp_object->session == NULL) {
		return SUCCESS;
	}

	ZVAL_STRINGL(&val, snmp_object->session->peername, strlen(snmp_object->session->peername));
	add_assoc_zval(retval, "hostname", &val);

	ZVAL_LONG(&val, snmp_object->session->remote_port);
	add_assoc_zval(retval, "port", &val);

	ZVAL_LONG(&val, snmp_object->session->timeout);
	add_assoc_zval(retval, "timeout", &val);

	ZVAL_LONG(&val, snmp_object->session->retries);
	add_assoc_zval(retval, "retries", &val);

	return SUCCESS;
}