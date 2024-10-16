static inline void php_var_serialize_long(smart_str *buf, zend_long val) /* {{{ */
{
	smart_str_appendl(buf, "i:", 2);
	smart_str_append_long(buf, val);
	smart_str_appendc(buf, ';');
}