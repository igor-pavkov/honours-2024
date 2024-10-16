PHPAPI void php_var_export(zval *struc, int level) /* {{{ */
{
	smart_str buf = {0};
	php_var_export_ex(struc, level, &buf);
	smart_str_0(&buf);
	PHPWRITE(ZSTR_VAL(buf.s), ZSTR_LEN(buf.s));
	smart_str_free(&buf);
}