PHP_METHOD(Phar, setSignatureAlgorithm)
{
	zend_long algo;
	char *error, *key = NULL;
	size_t key_len = 0;

	PHAR_ARCHIVE_OBJECT();

	if (PHAR_G(readonly) && !phar_obj->archive->is_data) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0,
			"Cannot set signature algorithm, phar is read-only");
		return;
	}

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS(), "l|s", &algo, &key, &key_len) != SUCCESS) {
		return;
	}
	if (ZEND_SIZE_T_INT_OVFL(key_len)) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0,
					"Cannot set signature algorithm, key too long");
		return;
	}

	switch (algo) {
		case PHAR_SIG_SHA256:
		case PHAR_SIG_SHA512:
#ifndef PHAR_HASH_OK
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0,
				"SHA-256 and SHA-512 signatures are only supported if the hash extension is enabled and built non-shared");
			return;
#endif
		case PHAR_SIG_MD5:
		case PHAR_SIG_SHA1:
		case PHAR_SIG_OPENSSL:
			if (phar_obj->archive->is_persistent && FAILURE == phar_copy_on_write(&(phar_obj->archive))) {
				zend_throw_exception_ex(phar_ce_PharException, 0, "phar \"%s\" is persistent, unable to copy on write", phar_obj->archive->fname);
				return;
			}
			phar_obj->archive->sig_flags = (php_uint32)algo;
			phar_obj->archive->is_modified = 1;
			PHAR_G(openssl_privatekey) = key;
			PHAR_G(openssl_privatekey_len) = (int)key_len;

			phar_flush(phar_obj->archive, 0, 0, 0, &error);
			if (error) {
				zend_throw_exception_ex(phar_ce_PharException, 0, "%s", error);
				efree(error);
			}
			break;
		default:
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0,
				"Unknown signature algorithm specified");
	}
}