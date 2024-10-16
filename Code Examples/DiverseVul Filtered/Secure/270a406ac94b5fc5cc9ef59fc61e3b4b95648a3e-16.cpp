static zend_bool php_openssl_validate_iv(char **piv, int *piv_len, int iv_required_len TSRMLS_DC)
{
	char *iv_new;

	/* Best case scenario, user behaved */
	if (*piv_len == iv_required_len) {
		return 0;
	}

	iv_new = ecalloc(1, iv_required_len + 1);

	if (*piv_len <= 0) {
		/* BC behavior */
		*piv_len = iv_required_len;
		*piv     = iv_new;
		return 1;
	}

	if (*piv_len < iv_required_len) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "IV passed is only %d bytes long, cipher expects an IV of precisely %d bytes, padding with \\0", *piv_len, iv_required_len);
		memcpy(iv_new, *piv, *piv_len);
		*piv_len = iv_required_len;
		*piv     = iv_new;
		return 1;
	}

	php_error_docref(NULL TSRMLS_CC, E_WARNING, "IV passed is %d bytes long which is longer than the %d expected by selected cipher, truncating", *piv_len, iv_required_len);
	memcpy(iv_new, *piv, iv_required_len);
	*piv_len = iv_required_len;
	*piv     = iv_new;
	return 1;

}