static int php_snmp_write_exceptions_enabled(php_snmp_object *snmp_object, zval *newval)
{
	zval ztmp;
	int ret = SUCCESS;
	if (Z_TYPE_P(newval) != IS_LONG) {
		ZVAL_COPY(&ztmp, newval);
		convert_to_long(&ztmp);
		newval = &ztmp;
	}

	snmp_object->exceptions_enabled = Z_LVAL_P(newval);

	if (newval == &ztmp) {
		zval_ptr_dtor(newval);
	}
	return ret;
}