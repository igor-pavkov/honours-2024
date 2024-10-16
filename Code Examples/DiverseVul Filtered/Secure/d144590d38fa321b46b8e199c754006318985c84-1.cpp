PHP_METHOD(Phar, addFile)
{
	char *fname, *localname = NULL;
	int fname_len, localname_len = 0;
	php_stream *resource;
	zval *zresource;

	PHAR_ARCHIVE_OBJECT();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "p|s", &fname, &fname_len, &localname, &localname_len) == FAILURE) {
		return;
	}

#if PHP_API_VERSION < 20100412
	if (PG(safe_mode) && (!php_checkuid(fname, NULL, CHECKUID_ALLOW_ONLY_FILE))) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "phar error: unable to open file \"%s\" to add to phar archive, safe_mode restrictions prevent this", fname);
		return;
	}
#endif

	if (!strstr(fname, "://") && php_check_open_basedir(fname TSRMLS_CC)) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "phar error: unable to open file \"%s\" to add to phar archive, open_basedir restrictions prevent this", fname);
		return;
	}

	if (!(resource = php_stream_open_wrapper(fname, "rb", 0, NULL))) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "phar error: unable to open file \"%s\" to add to phar archive", fname);
		return;
	}

	if (localname) {
		fname = localname;
		fname_len = localname_len;
	}

	MAKE_STD_ZVAL(zresource);
	php_stream_to_zval(resource, zresource);
	phar_add_file(&(phar_obj->arc.archive), fname, fname_len, NULL, 0, zresource TSRMLS_CC);
	efree(zresource);
	php_stream_close(resource);
}