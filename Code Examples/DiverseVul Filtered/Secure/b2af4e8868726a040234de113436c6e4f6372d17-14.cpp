static void php_var_serialize_class(smart_str *buf, zval *struc, zval *retval_ptr, php_serialize_data_t var_hash) /* {{{ */
{
	uint32_t count;
	zend_bool incomplete_class;
	HashTable *ht;

	incomplete_class = php_var_serialize_class_name(buf, struc);
	/* count after serializing name, since php_var_serialize_class_name
	 * changes the count if the variable is incomplete class */
	if (Z_TYPE_P(retval_ptr) == IS_ARRAY) {
		ht = Z_ARRVAL_P(retval_ptr);
		count = zend_array_count(ht);
	} else if (Z_TYPE_P(retval_ptr) == IS_OBJECT) {
		ht = Z_OBJPROP_P(retval_ptr);
		count = zend_array_count(ht);
		if (incomplete_class) {
			--count;
		}
	} else {
		count = 0;
		ht = NULL;
	}

	if (count > 0) {
		zval *d;
		zval nval, *nvalp;
		zend_string *name;
		HashTable *names, *propers;

		names = php_var_serialize_collect_names(ht, count, incomplete_class);

		smart_str_append_unsigned(buf, zend_hash_num_elements(names));
		smart_str_appendl(buf, ":{", 2);

		ZVAL_NULL(&nval);
		nvalp = &nval;
		propers = Z_OBJPROP_P(struc);

		ZEND_HASH_FOREACH_STR_KEY(names, name) {
			if ((d = zend_hash_find(propers, name)) != NULL) {
				if (Z_TYPE_P(d) == IS_INDIRECT) {
					d = Z_INDIRECT_P(d);
					if (Z_TYPE_P(d) == IS_UNDEF) {
						continue;
					}
				}
				php_var_serialize_string(buf, ZSTR_VAL(name), ZSTR_LEN(name));
				php_var_serialize_intern(buf, d, var_hash);
			} else {
				zend_class_entry *ce = Z_OBJ_P(struc)->ce;
				if (ce) {
					zend_string *prot_name, *priv_name;

					do {
						priv_name = zend_mangle_property_name(
								ZSTR_VAL(ce->name), ZSTR_LEN(ce->name), ZSTR_VAL(name), ZSTR_LEN(name), ce->type & ZEND_INTERNAL_CLASS);
						if ((d = zend_hash_find(propers, priv_name)) != NULL) {
							if (Z_TYPE_P(d) == IS_INDIRECT) {
								d = Z_INDIRECT_P(d);
								if (Z_ISUNDEF_P(d)) {
									break;
								}
							}
							php_var_serialize_string(buf, ZSTR_VAL(priv_name), ZSTR_LEN(priv_name));
							zend_string_free(priv_name);
							php_var_serialize_intern(buf, d, var_hash);
							break;
						}
						zend_string_free(priv_name);
						prot_name = zend_mangle_property_name(
								"*", 1, ZSTR_VAL(name), ZSTR_LEN(name), ce->type & ZEND_INTERNAL_CLASS);
						if ((d = zend_hash_find(propers, prot_name)) != NULL) {
							if (Z_TYPE_P(d) == IS_INDIRECT) {
								d = Z_INDIRECT_P(d);
								if (Z_TYPE_P(d) == IS_UNDEF) {
									zend_string_free(prot_name);
									break;
								}
							}
							php_var_serialize_string(buf, ZSTR_VAL(prot_name), ZSTR_LEN(prot_name));
							zend_string_free(prot_name);
							php_var_serialize_intern(buf, d, var_hash);
							break;
						}
						zend_string_free(prot_name);
						php_var_serialize_string(buf, ZSTR_VAL(name), ZSTR_LEN(name));
						php_var_serialize_intern(buf, nvalp, var_hash);
						php_error_docref(NULL, E_NOTICE,
								"\"%s\" returned as member variable from __sleep() but does not exist", ZSTR_VAL(name));
					} while (0);
				} else {
					php_var_serialize_string(buf, ZSTR_VAL(name), ZSTR_LEN(name));
					php_var_serialize_intern(buf, nvalp, var_hash);
				}
			}
		} ZEND_HASH_FOREACH_END();
		smart_str_appendc(buf, '}');

		zend_hash_destroy(names);
		FREE_HASHTABLE(names);
	} else {
		smart_str_appendl(buf, "0:{}", 4);
	}
}