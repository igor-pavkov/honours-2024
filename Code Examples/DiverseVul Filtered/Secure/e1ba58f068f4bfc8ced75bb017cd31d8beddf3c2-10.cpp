PHP_FUNCTION(parse_url)
{
	char *str;
	int str_len;
	php_url *resource;
	long key = -1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &str, &str_len, &key) == FAILURE) {
		return;
	}

	resource = php_url_parse_ex(str, str_len);
	if (resource == NULL) {
		/* @todo Find a method to determine why php_url_parse_ex() failed */
		RETURN_FALSE;
	}

	if (key > -1) {
		switch (key) {
			case PHP_URL_SCHEME:
				if (resource->scheme != NULL) RETVAL_STRING(resource->scheme, 1);
				break;
			case PHP_URL_HOST:
				if (resource->host != NULL) RETVAL_STRING(resource->host, 1);
				break;
			case PHP_URL_PORT:
				if (resource->port != 0) RETVAL_LONG(resource->port);
				break;
			case PHP_URL_USER:
				if (resource->user != NULL) RETVAL_STRING(resource->user, 1);
				break;
			case PHP_URL_PASS:
				if (resource->pass != NULL) RETVAL_STRING(resource->pass, 1);
				break;
			case PHP_URL_PATH:
				if (resource->path != NULL) RETVAL_STRING(resource->path, 1);
				break;
			case PHP_URL_QUERY:
				if (resource->query != NULL) RETVAL_STRING(resource->query, 1);
				break;
			case PHP_URL_FRAGMENT:
				if (resource->fragment != NULL) RETVAL_STRING(resource->fragment, 1);
				break;
			default:
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid URL component identifier %ld", key);
				RETVAL_FALSE;
		}
		goto done;
	}

	/* allocate an array for return */
	array_init(return_value);

    /* add the various elements to the array */
	if (resource->scheme != NULL)
		add_assoc_string(return_value, "scheme", resource->scheme, 1);
	if (resource->host != NULL)
		add_assoc_string(return_value, "host", resource->host, 1);
	if (resource->port != 0)
		add_assoc_long(return_value, "port", resource->port);
	if (resource->user != NULL)
		add_assoc_string(return_value, "user", resource->user, 1);
	if (resource->pass != NULL)
		add_assoc_string(return_value, "pass", resource->pass, 1);
	if (resource->path != NULL)
		add_assoc_string(return_value, "path", resource->path, 1);
	if (resource->query != NULL)
		add_assoc_string(return_value, "query", resource->query, 1);
	if (resource->fragment != NULL)
		add_assoc_string(return_value, "fragment", resource->fragment, 1);
done:
	php_url_free(resource);
}