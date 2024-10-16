PHPAPI void php_var_export_ex(zval *struc, int level, smart_str *buf) /* {{{ */
{
	HashTable *myht;
	char *tmp_str;
	size_t tmp_len;
	zend_string *ztmp, *ztmp2;
	zend_ulong index;
	zend_string *key;
	zval *val;

again:
	switch (Z_TYPE_P(struc)) {
		case IS_FALSE:
			smart_str_appendl(buf, "false", 5);
			break;
		case IS_TRUE:
			smart_str_appendl(buf, "true", 4);
			break;
		case IS_NULL:
			smart_str_appendl(buf, "NULL", 4);
			break;
		case IS_LONG:
			smart_str_append_long(buf, Z_LVAL_P(struc));
			break;
		case IS_DOUBLE:
			tmp_len = spprintf(&tmp_str, 0,"%.*H", PG(serialize_precision), Z_DVAL_P(struc));
			smart_str_appendl(buf, tmp_str, tmp_len);
			/* Without a decimal point, PHP treats a number literal as an int.
			 * This check even works for scientific notation, because the
			 * mantissa always contains a decimal point.
			 * We need to check for finiteness, because INF, -INF and NAN
			 * must not have a decimal point added.
			 */
			if (zend_finite(Z_DVAL_P(struc)) && NULL == strchr(tmp_str, '.')) {
				smart_str_appendl(buf, ".0", 2);
			}
			efree(tmp_str);
			break;
		case IS_STRING:
			ztmp = php_addcslashes(Z_STR_P(struc), 0, "'\\", 2);
			ztmp2 = php_str_to_str(ZSTR_VAL(ztmp), ZSTR_LEN(ztmp), "\0", 1, "' . \"\\0\" . '", 12);

			smart_str_appendc(buf, '\'');
			smart_str_append(buf, ztmp2);
			smart_str_appendc(buf, '\'');

			zend_string_free(ztmp);
			zend_string_free(ztmp2);
			break;
		case IS_ARRAY:
			myht = Z_ARRVAL_P(struc);
			if (ZEND_HASH_APPLY_PROTECTION(myht) && myht->u.v.nApplyCount++ > 0) {
				myht->u.v.nApplyCount--;
				smart_str_appendl(buf, "NULL", 4);
				zend_error(E_WARNING, "var_export does not handle circular references");
				return;
			}
			if (level > 1) {
				smart_str_appendc(buf, '\n');
				buffer_append_spaces(buf, level - 1);
			}
			smart_str_appendl(buf, "array (\n", 8);
			ZEND_HASH_FOREACH_KEY_VAL_IND(myht, index, key, val) {
				php_array_element_export(val, index, key, level, buf);
			} ZEND_HASH_FOREACH_END();
			if (ZEND_HASH_APPLY_PROTECTION(myht)) {
				myht->u.v.nApplyCount--;
			}
			if (level > 1) {
				buffer_append_spaces(buf, level - 1);
			}
			smart_str_appendc(buf, ')');

			break;

		case IS_OBJECT:
			myht = Z_OBJPROP_P(struc);
			if (myht) {
				if (myht->u.v.nApplyCount > 0) {
					smart_str_appendl(buf, "NULL", 4);
					zend_error(E_WARNING, "var_export does not handle circular references");
					return;
				} else {
					myht->u.v.nApplyCount++;
				}
			}
			if (level > 1) {
				smart_str_appendc(buf, '\n');
				buffer_append_spaces(buf, level - 1);
			}

			smart_str_append(buf, Z_OBJCE_P(struc)->name);
			smart_str_appendl(buf, "::__set_state(array(\n", 21);

			if (myht) {
				ZEND_HASH_FOREACH_KEY_VAL_IND(myht, index, key, val) {
					php_object_element_export(val, index, key, level, buf);
				} ZEND_HASH_FOREACH_END();
				myht->u.v.nApplyCount--;
			}
			if (level > 1) {
				buffer_append_spaces(buf, level - 1);
			}
			smart_str_appendl(buf, "))", 2);

			break;
		case IS_REFERENCE:
			struc = Z_REFVAL_P(struc);
			goto again;
			break;
		default:
			smart_str_appendl(buf, "NULL", 4);
			break;
	}
}