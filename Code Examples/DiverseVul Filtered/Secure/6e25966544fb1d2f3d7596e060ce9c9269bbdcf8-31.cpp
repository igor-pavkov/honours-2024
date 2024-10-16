zval *php_snmp_read_property(zval *object, zval *member, int type, void **cache_slot, zval *rv)
{
	zval tmp_member;
	zval *retval;
	php_snmp_object *obj;
	php_snmp_prop_handler *hnd;
	int ret;

	obj = Z_SNMP_P(object);

	if (Z_TYPE_P(member) != IS_STRING) {
		ZVAL_COPY(&tmp_member, member);
		convert_to_string(&tmp_member);
		member = &tmp_member;
	}

	hnd = zend_hash_find_ptr(&php_snmp_properties, Z_STR_P(member));

	if (hnd && hnd->read_func) {
		ret = hnd->read_func(obj, rv);
		if (ret == SUCCESS) {
			retval = rv;
		} else {
			retval = &EG(uninitialized_zval);
		}
	} else {
		zend_object_handlers * std_hnd = zend_get_std_object_handlers();
		retval = std_hnd->read_property(object, member, type, cache_slot, rv);
	}

	if (member == &tmp_member) {
		zval_ptr_dtor(member);
	}

	return retval;
}