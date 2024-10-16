static void php_object_property_dump(zval *zv, zend_ulong index, zend_string *key, int level) /* {{{ */
{
	const char *prop_name, *class_name;

	if (key == NULL) { /* numeric key */
		php_printf("%*c[" ZEND_LONG_FMT "]=>\n", level + 1, ' ', index);
	} else { /* string key */
		int unmangle = zend_unmangle_property_name(key, &class_name, &prop_name);
		php_printf("%*c[", level + 1, ' ');

		if (class_name && unmangle == SUCCESS) {
			if (class_name[0] == '*') {
				php_printf("\"%s\":protected", prop_name);
			} else {
				php_printf("\"%s\":\"%s\":private", prop_name, class_name);
			}
		} else {
			php_printf("\"");
			PHPWRITE(ZSTR_VAL(key), ZSTR_LEN(key));
			php_printf("\"");
		}
		ZEND_PUTS("]=>\n");
	}
	php_var_dump(zv, level + 2);
}