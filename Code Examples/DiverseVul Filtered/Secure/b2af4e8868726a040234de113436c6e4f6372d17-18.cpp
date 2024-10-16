static inline zend_bool php_var_serialize_class_name(smart_str *buf, zval *struc) /* {{{ */
{
	PHP_CLASS_ATTRIBUTES;

	PHP_SET_CLASS_ATTRIBUTES(struc);
	smart_str_appendl(buf, "O:", 2);
	smart_str_append_unsigned(buf, ZSTR_LEN(class_name));
	smart_str_appendl(buf, ":\"", 2);
	smart_str_append(buf, class_name);
	smart_str_appendl(buf, "\":", 2);
	PHP_CLEANUP_CLASS_ATTRIBUTES();
	return incomplete_class;
}