static int php_snmp_write_info(php_snmp_object *snmp_object, zval *newval)
{
	php_error_docref(NULL, E_WARNING, "info property is read-only");
	return FAILURE;
}