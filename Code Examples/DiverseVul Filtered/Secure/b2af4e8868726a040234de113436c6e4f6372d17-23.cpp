PHPAPI void php_var_serialize(smart_str *buf, zval *struc, php_serialize_data_t *data) /* {{{ */
{
	php_var_serialize_intern(buf, struc, *data);
	smart_str_0(buf);
}