PHP_METHOD(Phar, setSignatureAlgorithm)
{
	long algo;
	char *error, *key = NULL;
	int key_len = 0;

	PHAR_ARCHIVE_OBJECT();

	if (PHAR_G(readonly) && !phar_obj->arc.archive->is_data) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
			"Cannot set signature algorithm, phar is read-only");
		return;
	}

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "l|s", &algo, &key, &key_len) != SUCCESS) {
		return;
	}

	switch (algo) {
		case PHAR_SIG_SHA256:
		case PHAR_SIG_SHA512:
#ifndef PHAR_HASH_OK
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
				"SHA-256 and SHA-512 signatures are only supported if the hash extension is enabled and built non-shared");
			return;
#endif
		case PHAR_SIG_MD5:
		case PHAR_SIG_SHA1:
		case PHAR_SIG_OPENSSL:
			if (phar_obj->arc.archive->is_persistent && FAILURE == phar_copy_on_write(&(phar_obj->arc.archive) TSRMLS_CC)) {
				zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar \"%s\" is persistent, unable to copy on write", phar_obj->arc.archive->fname);
				return;
			}
			phar_obj->arc.archive->sig_flags = algo;
			phar_obj->arc.archive->is_modified = 1;
			PHAR_G(openssl_privatekey) = key;
			PHAR_G(openssl_privatekey_len) = key_len;

			phar_flush(phar_obj->arc.archive, 0, 0, 0, &error TSRMLS_CC);
			if (error) {
				zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
				efree(error);
			}
			break;
		default:
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
				"Unknown signature algorithm specified");
	}
}