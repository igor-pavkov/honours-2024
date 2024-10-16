PHP_METHOD(Phar, addFile)
{
	char *fname, *localname = NULL;
	size_t fname_len, localname_len = 0;
	php_stream *resource;
	zval zresource;

	PHAR_ARCHIVE_OBJECT();

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "p|s", &fname, &fname_len, &localname, &localname_len) == FAILURE) {
		return;
	}
	if (ZEND_SIZE_T_INT_OVFL(fname_len)) {
		RETURN_FALSE;
	}

	if (!strstr(fname, "://") && php_check_open_basedir(fname)) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0, "phar error: unable to open file \"%s\" to add to phar archive, open_basedir restrictions prevent this", fname);
		return;
	}

	if (!(resource = php_stream_open_wrapper(fname, "rb", 0, NULL))) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0, "phar error: unable to open file \"%s\" to add to phar archive", fname);
		return;
	}

	if (localname) {
		fname = localname;
		fname_len = localname_len;
	}

	php_stream_to_zval(resource, &zresource);
	phar_add_file(&(phar_obj->archive), fname, (int)fname_len, NULL, 0, &zresource);
	zval_ptr_dtor(&zresource);
}