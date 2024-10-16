static inline void php_var_serialize_string(smart_str *buf, char *str, size_t len) /* {{{ */
{
	smart_str_appendl(buf, "s:", 2);
	smart_str_append_unsigned(buf, len);
	smart_str_appendl(buf, ":\"", 2);
	smart_str_appendl(buf, str, len);
	smart_str_appendl(buf, "\";", 2);
}