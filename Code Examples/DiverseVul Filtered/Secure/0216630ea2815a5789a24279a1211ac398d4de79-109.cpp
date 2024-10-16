void php_openssl_store_errors()
{
	struct php_openssl_errors *errors;
	int error_code = ERR_get_error();

	if (!error_code) {
		return;
	}

	if (!OPENSSL_G(errors)) {
		OPENSSL_G(errors) = pecalloc(1, sizeof(struct php_openssl_errors), 1);
	}

	errors = OPENSSL_G(errors);

	do {
		errors->top = (errors->top + 1) % ERR_NUM_ERRORS;
		if (errors->top == errors->bottom) {
			errors->bottom = (errors->bottom + 1) % ERR_NUM_ERRORS;
		}
		errors->buffer[errors->top] = error_code;
	} while ((error_code = ERR_get_error()));

}