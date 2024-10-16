static void php_var_serialize_intern(smart_str *buf, zval *struc, php_serialize_data_t var_hash) /* {{{ */
{
	zend_long var_already;
	HashTable *myht;

	if (EG(exception)) {
		return;
	}

	if (var_hash && (var_already = php_add_var_hash(var_hash, struc))) {
		if (Z_ISREF_P(struc)) {
			smart_str_appendl(buf, "R:", 2);
			smart_str_append_long(buf, var_already);
			smart_str_appendc(buf, ';');
			return;
		} else if (Z_TYPE_P(struc) == IS_OBJECT) {
			smart_str_appendl(buf, "r:", 2);
			smart_str_append_long(buf, var_already);
			smart_str_appendc(buf, ';');
			return;
		}
	}

again:
	switch (Z_TYPE_P(struc)) {
		case IS_FALSE:
			smart_str_appendl(buf, "b:0;", 4);
			return;

		case IS_TRUE:
			smart_str_appendl(buf, "b:1;", 4);
			return;

		case IS_NULL:
			smart_str_appendl(buf, "N;", 2);
			return;

		case IS_LONG:
			php_var_serialize_long(buf, Z_LVAL_P(struc));
			return;

		case IS_DOUBLE: {
				char *s;

				smart_str_appendl(buf, "d:", 2);
				s = (char *) safe_emalloc(PG(serialize_precision), 1, MAX_LENGTH_OF_DOUBLE + 1);
				php_gcvt(Z_DVAL_P(struc), (int)PG(serialize_precision), '.', 'E', s);
				smart_str_appends(buf, s);
				smart_str_appendc(buf, ';');
				efree(s);
				return;
			}

		case IS_STRING:
			php_var_serialize_string(buf, Z_STRVAL_P(struc), Z_STRLEN_P(struc));
			return;

		case IS_OBJECT: {
				zval retval;
				zval fname;
				int res;
				zend_class_entry *ce = Z_OBJCE_P(struc);

				if (ce->serialize != NULL) {
					/* has custom handler */
					unsigned char *serialized_data = NULL;
					size_t serialized_length;

					if (ce->serialize(struc, &serialized_data, &serialized_length, (zend_serialize_data *)var_hash) == SUCCESS) {
						smart_str_appendl(buf, "C:", 2);
						smart_str_append_unsigned(buf, ZSTR_LEN(Z_OBJCE_P(struc)->name));
						smart_str_appendl(buf, ":\"", 2);
						smart_str_append(buf, Z_OBJCE_P(struc)->name);
						smart_str_appendl(buf, "\":", 2);

						smart_str_append_unsigned(buf, serialized_length);
						smart_str_appendl(buf, ":{", 2);
						smart_str_appendl(buf, (char *) serialized_data, serialized_length);
						smart_str_appendc(buf, '}');
					} else {
						smart_str_appendl(buf, "N;", 2);
					}
					if (serialized_data) {
						efree(serialized_data);
					}
					return;
				}

				if (ce != PHP_IC_ENTRY && zend_hash_str_exists(&ce->function_table, "__sleep", sizeof("__sleep")-1)) {
					ZVAL_STRINGL(&fname, "__sleep", sizeof("__sleep") - 1);
					BG(serialize_lock)++;
					res = call_user_function_ex(CG(function_table), struc, &fname, &retval, 0, 0, 1, NULL);
					BG(serialize_lock)--;
					zval_dtor(&fname);

					if (EG(exception)) {
						zval_ptr_dtor(&retval);
						return;
					}

					if (res == SUCCESS) {
						if (Z_TYPE(retval) != IS_UNDEF) {
							if (HASH_OF(&retval)) {
								php_var_serialize_class(buf, struc, &retval, var_hash);
							} else {
								php_error_docref(NULL, E_NOTICE, "__sleep should return an array only containing the names of instance-variables to serialize");
								/* we should still add element even if it's not OK,
								 * since we already wrote the length of the array before */
								smart_str_appendl(buf,"N;", 2);
							}
							zval_ptr_dtor(&retval);
						}
						return;
					}
					zval_ptr_dtor(&retval);
				}

				/* fall-through */
			}
		case IS_ARRAY: {
			uint32_t i;
			zend_bool incomplete_class = 0;
			if (Z_TYPE_P(struc) == IS_ARRAY) {
				smart_str_appendl(buf, "a:", 2);
				myht = Z_ARRVAL_P(struc);
				i = zend_array_count(myht);
			} else {
				incomplete_class = php_var_serialize_class_name(buf, struc);
				myht = Z_OBJPROP_P(struc);
				/* count after serializing name, since php_var_serialize_class_name
				 * changes the count if the variable is incomplete class */
				i = zend_array_count(myht);
				if (i > 0 && incomplete_class) {
					--i;
				}
			}
			smart_str_append_unsigned(buf, i);
			smart_str_appendl(buf, ":{", 2);
			if (i > 0) {
				zend_string *key;
				zval *data;
				zend_ulong index;

				ZEND_HASH_FOREACH_KEY_VAL_IND(myht, index, key, data) {

					if (incomplete_class && strcmp(ZSTR_VAL(key), MAGIC_MEMBER) == 0) {
						continue;
					}

					if (!key) {
						php_var_serialize_long(buf, index);
					} else {
						php_var_serialize_string(buf, ZSTR_VAL(key), ZSTR_LEN(key));
					}

					if (Z_ISREF_P(data) && Z_REFCOUNT_P(data) == 1) {
						data = Z_REFVAL_P(data);
					}

					/* we should still add element even if it's not OK,
					 * since we already wrote the length of the array before */
					if ((Z_TYPE_P(data) == IS_ARRAY && Z_TYPE_P(struc) == IS_ARRAY && Z_ARR_P(data) == Z_ARR_P(struc))
						|| (Z_TYPE_P(data) == IS_ARRAY && Z_ARRVAL_P(data)->u.v.nApplyCount > 1)
					) {
						smart_str_appendl(buf, "N;", 2);
					} else {
						if (Z_TYPE_P(data) == IS_ARRAY && ZEND_HASH_APPLY_PROTECTION(Z_ARRVAL_P(data))) {
							Z_ARRVAL_P(data)->u.v.nApplyCount++;
						}
						php_var_serialize_intern(buf, data, var_hash);
						if (Z_TYPE_P(data) == IS_ARRAY && ZEND_HASH_APPLY_PROTECTION(Z_ARRVAL_P(data))) {
							Z_ARRVAL_P(data)->u.v.nApplyCount--;
						}
					}
				} ZEND_HASH_FOREACH_END();
			}
			smart_str_appendc(buf, '}');
			return;
		}
		case IS_REFERENCE:
			struc = Z_REFVAL_P(struc);
			goto again;
		default:
			smart_str_appendl(buf, "i:0;", 4);
			return;
	}
}