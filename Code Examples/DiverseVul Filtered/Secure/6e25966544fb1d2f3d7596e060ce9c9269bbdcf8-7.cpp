static int php_snmp_write_max_oids(php_snmp_object *snmp_object, zval *newval)
{
	zval ztmp;
	int ret = SUCCESS;

	if (Z_TYPE_P(newval) == IS_NULL) {
		snmp_object->max_oids = 0;
		return ret;
	}

	if (Z_TYPE_P(newval) != IS_LONG) {
		ztmp = *newval;
		zval_copy_ctor(&ztmp);
		convert_to_long(&ztmp);
		newval = &ztmp;
	}

	if (Z_LVAL_P(newval) > 0) {
		snmp_object->max_oids = Z_LVAL_P(newval);
	} else {
		php_error_docref(NULL, E_WARNING, "max_oids should be positive integer or NULL, got %pd", Z_LVAL_P(newval));
	}

	if (newval == &ztmp) {
		zval_dtor(newval);
	}

	return ret;
}