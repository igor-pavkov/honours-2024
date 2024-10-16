int proxy_authentication(zval* this_ptr, smart_str* soap_headers)
{
	zval *login, *password;

	if ((login = zend_hash_str_find(Z_OBJPROP_P(this_ptr), "_proxy_login", sizeof("_proxy_login")-1)) != NULL &&
	    Z_TYPE_P(login) == IS_STRING) {
		zend_string *buf;
		smart_str auth = {0};

		smart_str_appendl(&auth, Z_STRVAL_P(login), Z_STRLEN_P(login));
		smart_str_appendc(&auth, ':');
		if ((password = zend_hash_str_find(Z_OBJPROP_P(this_ptr), "_proxy_password", sizeof("_proxy_password")-1)) != NULL &&
		    Z_TYPE_P(password) == IS_STRING) {
			smart_str_appendl(&auth, Z_STRVAL_P(password), Z_STRLEN_P(password));
		}
		smart_str_0(&auth);
		buf = php_base64_encode((unsigned char*)ZSTR_VAL(auth.s), ZSTR_LEN(auth.s));
		smart_str_append_const(soap_headers, "Proxy-Authorization: Basic ");
		smart_str_appendl(soap_headers, (char*)ZSTR_VAL(buf), ZSTR_LEN(buf));
		smart_str_append_const(soap_headers, "\r\n");
		zend_string_release(buf);
		smart_str_free(&auth);
		return 1;
	}
	return 0;
}