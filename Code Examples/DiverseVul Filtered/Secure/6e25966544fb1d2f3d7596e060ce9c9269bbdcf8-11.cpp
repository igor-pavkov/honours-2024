PHP_METHOD(snmp, getErrno)
{
	php_snmp_object *snmp_object;
	zval *object = getThis();

	snmp_object = Z_SNMP_P(object);

	RETVAL_LONG(snmp_object->snmp_errno);
	return;
}