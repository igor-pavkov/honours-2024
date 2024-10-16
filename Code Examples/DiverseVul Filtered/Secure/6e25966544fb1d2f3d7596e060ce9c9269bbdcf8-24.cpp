PHP_METHOD(snmp, getError)
{
	php_snmp_object *snmp_object;
	zval *object = getThis();

	snmp_object = Z_SNMP_P(object);

	RETURN_STRING(snmp_object->snmp_errstr);
}