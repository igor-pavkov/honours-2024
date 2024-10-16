static void zval_array_element_dump(zval *zv, zend_ulong index, zend_string *key, int level) /* {{{ */
{
	if (key == NULL) { /* numeric key */
		php_printf("%*c[" ZEND_LONG_FMT "]=>\n", level + 1, ' ', index);
	} else { /* string key */
		php_printf("%*c[\"", level + 1, ' ');
		PHPWRITE(ZSTR_VAL(key), ZSTR_LEN(key));
		php_printf("\"]=>\n");
	}
	php_debug_zval_dump(zv, level + 2);
}