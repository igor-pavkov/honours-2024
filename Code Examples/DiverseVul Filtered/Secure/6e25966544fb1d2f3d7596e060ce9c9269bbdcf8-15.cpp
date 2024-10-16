static int php_snmp_read_max_oids(php_snmp_object *snmp_object, zval *retval)
{
	if (snmp_object->max_oids > 0) {
		ZVAL_LONG(retval, snmp_object->max_oids);
	} else {
		ZVAL_NULL(retval);
	}
	return SUCCESS;
}