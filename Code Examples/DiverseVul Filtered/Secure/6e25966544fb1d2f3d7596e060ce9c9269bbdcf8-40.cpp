void php_snmp_write_property(zval *object, zval *member, zval *value, void **cache_slot)
{
	zval tmp_member;
	php_snmp_object *obj;
	php_snmp_prop_handler *hnd;

	if (Z_TYPE_P(member) != IS_STRING) {
		ZVAL_COPY(&tmp_member, member);
		convert_to_string(&tmp_member);
		member = &tmp_member;
	}

	obj = Z_SNMP_P(object);

	hnd = zend_hash_find_ptr(&php_snmp_properties, Z_STR_P(member));

	if (hnd && hnd->write_func) {
		hnd->write_func(obj, value);
		/*
		if (!PZVAL_IS_REF(value) && Z_REFCOUNT_P(value) == 0) {
			Z_ADDREF_P(value);
			zval_ptr_dtor(&value);
		}
		*/
	} else {
		zend_object_handlers * std_hnd = zend_get_std_object_handlers();
		std_hnd->write_property(object, member, value, cache_slot);
	}

	if (member == &tmp_member) {
		zval_ptr_dtor(member);
	}
}