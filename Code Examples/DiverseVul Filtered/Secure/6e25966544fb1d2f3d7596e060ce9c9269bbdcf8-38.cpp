static int php_snmp_has_property(zval *object, zval *member, int has_set_exists, void **cache_slot)
{
	zval rv;
	php_snmp_prop_handler *hnd;
	int ret = 0;

	if ((hnd = zend_hash_find_ptr(&php_snmp_properties, Z_STR_P(member))) != NULL) {
		switch (has_set_exists) {
			case 2:
				ret = 1;
				break;
			case 0: {
				zval *value = php_snmp_read_property(object, member, BP_VAR_IS, cache_slot, &rv);
				if (value != &EG(uninitialized_zval)) {
					ret = Z_TYPE_P(value) != IS_NULL? 1 : 0;
					zval_ptr_dtor(value);
				}
				break;
			}
			default: {
				zval *value = php_snmp_read_property(object, member, BP_VAR_IS, cache_slot, &rv);
				if (value != &EG(uninitialized_zval)) {
					convert_to_boolean(value);
					ret = Z_TYPE_P(value) == IS_TRUE? 1:0;
				}
				break;
			}
		}
	} else {
		zend_object_handlers *std_hnd = zend_get_std_object_handlers();
		ret = std_hnd->has_property(object, member, has_set_exists, cache_slot);
	}
	return ret;
}