static HashTable *php_snmp_get_properties(zval *object)
{
	php_snmp_object *obj;
	php_snmp_prop_handler *hnd;
	HashTable *props;
	zval rv;
	zend_string *key;
	zend_ulong num_key;

	obj = Z_SNMP_P(object);
	props = zend_std_get_properties(object);

	ZEND_HASH_FOREACH_KEY_PTR(&php_snmp_properties, num_key, key, hnd) {
		if (!hnd->read_func || hnd->read_func(obj, &rv) != SUCCESS) {
			ZVAL_NULL(&rv);
		}
		zend_hash_update(props, key, &rv);
	} ZEND_HASH_FOREACH_END();

	return obj->zo.properties;
}