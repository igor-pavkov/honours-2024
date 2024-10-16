PHP_FUNCTION(parse_url)
{
	char *str;
	size_t str_len;
	php_url *resource;
	zend_long key = -1;
	zval tmp;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STRING(str, str_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(key)
	ZEND_PARSE_PARAMETERS_END();

	resource = php_url_parse_ex(str, str_len);
	if (resource == NULL) {
		/* @todo Find a method to determine why php_url_parse_ex() failed */
		RETURN_FALSE;
	}

	if (key > -1) {
		switch (key) {
			case PHP_URL_SCHEME:
				if (resource->scheme != NULL) RETVAL_STRING(resource->scheme);
				break;
			case PHP_URL_HOST:
				if (resource->host != NULL) RETVAL_STRING(resource->host);
				break;
			case PHP_URL_PORT:
				if (resource->port != 0) RETVAL_LONG(resource->port);
				break;
			case PHP_URL_USER:
				if (resource->user != NULL) RETVAL_STRING(resource->user);
				break;
			case PHP_URL_PASS:
				if (resource->pass != NULL) RETVAL_STRING(resource->pass);
				break;
			case PHP_URL_PATH:
				if (resource->path != NULL) RETVAL_STRING(resource->path);
				break;
			case PHP_URL_QUERY:
				if (resource->query != NULL) RETVAL_STRING(resource->query);
				break;
			case PHP_URL_FRAGMENT:
				if (resource->fragment != NULL) RETVAL_STRING(resource->fragment);
				break;
			default:
				php_error_docref(NULL, E_WARNING, "Invalid URL component identifier " ZEND_LONG_FMT, key);
				RETVAL_FALSE;
		}
		goto done;
	}

	/* allocate an array for return */
	array_init(return_value);

    /* add the various elements to the array */
	if (resource->scheme != NULL) {
		ZVAL_STRING(&tmp, resource->scheme);
		zend_hash_add_new(Z_ARRVAL_P(return_value), ZSTR_KNOWN(ZEND_STR_SCHEME), &tmp);
	}
	if (resource->host != NULL) {
		ZVAL_STRING(&tmp, resource->host);
		zend_hash_add_new(Z_ARRVAL_P(return_value), ZSTR_KNOWN(ZEND_STR_HOST), &tmp);
	}
	if (resource->port != 0) {
		ZVAL_LONG(&tmp, resource->port);
		zend_hash_add_new(Z_ARRVAL_P(return_value), ZSTR_KNOWN(ZEND_STR_PORT), &tmp);
	}
	if (resource->user != NULL) {
		ZVAL_STRING(&tmp, resource->user);
		zend_hash_add_new(Z_ARRVAL_P(return_value), ZSTR_KNOWN(ZEND_STR_USER), &tmp);
	}
	if (resource->pass != NULL) {
		ZVAL_STRING(&tmp, resource->pass);
		zend_hash_add_new(Z_ARRVAL_P(return_value), ZSTR_KNOWN(ZEND_STR_PASS), &tmp);
	}
	if (resource->path != NULL) {
		ZVAL_STRING(&tmp, resource->path);
		zend_hash_add_new(Z_ARRVAL_P(return_value), ZSTR_KNOWN(ZEND_STR_PATH), &tmp);
	}
	if (resource->query != NULL) {
		ZVAL_STRING(&tmp, resource->query);
		zend_hash_add_new(Z_ARRVAL_P(return_value), ZSTR_KNOWN(ZEND_STR_QUERY), &tmp);
	}
	if (resource->fragment != NULL) {
		ZVAL_STRING(&tmp, resource->fragment);
		zend_hash_add_new(Z_ARRVAL_P(return_value), ZSTR_KNOWN(ZEND_STR_FRAGMENT), &tmp);
	}
done:
	php_url_free(resource);
}