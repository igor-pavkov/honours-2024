SPL_METHOD(DirectoryIterator, getExtension)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(getThis());
	const char *p;
	size_t idx;
	zend_string *fname;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	fname = php_basename(intern->u.dir.entry.d_name, strlen(intern->u.dir.entry.d_name), NULL, 0);

	p = zend_memrchr(ZSTR_VAL(fname), '.', ZSTR_LEN(fname));
	if (p) {
		idx = p - ZSTR_VAL(fname);
		RETVAL_STRINGL(ZSTR_VAL(fname) + idx + 1, ZSTR_LEN(fname) - idx - 1);
		zend_string_release(fname);
	} else {
		zend_string_release(fname);
		RETURN_EMPTY_STRING();
	}
}