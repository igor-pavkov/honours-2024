SPL_METHOD(SplFileInfo, __construct)
{
	spl_filesystem_object *intern;
	char *path;
	size_t len;

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "p", &path, &len) == FAILURE) {
		return;
	}

	intern = Z_SPLFILESYSTEM_P(getThis());

	spl_filesystem_info_set_filename(intern, path, len, 1);

	/* intern->type = SPL_FS_INFO; already set */
}