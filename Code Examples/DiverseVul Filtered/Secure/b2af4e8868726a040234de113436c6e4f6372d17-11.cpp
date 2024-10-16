static void php_array_element_export(zval *zv, zend_ulong index, zend_string *key, int level, smart_str *buf) /* {{{ */
{
	if (key == NULL) { /* numeric key */
		buffer_append_spaces(buf, level+1);
		smart_str_append_long(buf, (zend_long) index);
		smart_str_appendl(buf, " => ", 4);

	} else { /* string key */
		zend_string *tmp_str;
		zend_string *ckey = php_addcslashes(key, 0, "'\\", 2);
		tmp_str = php_str_to_str(ZSTR_VAL(ckey), ZSTR_LEN(ckey), "\0", 1, "' . \"\\0\" . '", 12);

		buffer_append_spaces(buf, level + 1);

		smart_str_appendc(buf, '\'');
		smart_str_append(buf, tmp_str);
		smart_str_appendl(buf, "' => ", 5);

		zend_string_free(ckey);
		zend_string_free(tmp_str);
	}
	php_var_export_ex(zv, level + 2, buf);

	smart_str_appendc(buf, ',');
	smart_str_appendc(buf, '\n');
}