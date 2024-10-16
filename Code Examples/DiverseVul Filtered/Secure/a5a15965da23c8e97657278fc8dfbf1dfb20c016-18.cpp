SPL_METHOD(SplFileInfo, getExtension)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(getThis());
	char *fname = NULL;
	const char *p;
	size_t flen;
	size_t path_len;
	size_t idx;
	zend_string *ret;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	spl_filesystem_object_get_path(intern, &path_len);

	if (path_len && path_len < intern->file_name_len) {
		fname = intern->file_name + path_len + 1;
		flen = intern->file_name_len - (path_len + 1);
	} else {
		fname = intern->file_name;
		flen = intern->file_name_len;
	}

	ret = php_basename(fname, flen, NULL, 0);

	p = zend_memrchr(ZSTR_VAL(ret), '.', ZSTR_LEN(ret));
	if (p) {
		idx = p - ZSTR_VAL(ret);
		RETVAL_STRINGL(ZSTR_VAL(ret) + idx + 1, ZSTR_LEN(ret) - idx - 1);
		zend_string_release(ret);
		return;
	} else {
		zend_string_release(ret);
		RETURN_EMPTY_STRING();
	}
}