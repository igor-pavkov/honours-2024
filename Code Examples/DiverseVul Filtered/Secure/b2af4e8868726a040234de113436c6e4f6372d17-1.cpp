static HashTable *php_var_serialize_collect_names(HashTable *src, uint32_t count, zend_bool incomplete) /* {{{ */ {
	zval *val;
	HashTable *ht;
	zend_string *key, *name;

	ALLOC_HASHTABLE(ht);
	zend_hash_init(ht, count, NULL, NULL, 0);
	ZEND_HASH_FOREACH_STR_KEY_VAL(src, key, val) {
		if (incomplete && strcmp(ZSTR_VAL(key), MAGIC_MEMBER) == 0) {
			continue;
		}
		if (Z_TYPE_P(val) != IS_STRING) {
			php_error_docref(NULL, E_NOTICE,
					"__sleep should return an array only containing the names of instance-variables to serialize.");
		}
		name = zval_get_string(val);
		if (zend_hash_exists(ht, name)) {
			php_error_docref(NULL, E_NOTICE,
					"\"%s\" is returned from __sleep multiple times", ZSTR_VAL(name));
			zend_string_release(name);
			continue;
		}
		zend_hash_add_empty_element(ht, name);
		zend_string_release(name);
	} ZEND_HASH_FOREACH_END();

	return ht;
}