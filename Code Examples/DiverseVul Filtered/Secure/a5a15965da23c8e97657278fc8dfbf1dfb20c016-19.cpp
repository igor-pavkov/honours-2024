SPL_METHOD(SplFileInfo, getBasename)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(getThis());
	char *fname, *suffix = 0;
	size_t flen;
	size_t slen = 0, path_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|s", &suffix, &slen) == FAILURE) {
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

	RETURN_STR(php_basename(fname, flen, suffix, slen));
}