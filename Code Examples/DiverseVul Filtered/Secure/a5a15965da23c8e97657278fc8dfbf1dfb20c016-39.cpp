SPL_METHOD(DirectoryIterator, getBasename)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(getThis());
	char *suffix = 0;
	size_t slen = 0;
	zend_string *fname;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|s", &suffix, &slen) == FAILURE) {
		return;
	}

	fname = php_basename(intern->u.dir.entry.d_name, strlen(intern->u.dir.entry.d_name), suffix, slen);

	RETVAL_STR(fname);
}