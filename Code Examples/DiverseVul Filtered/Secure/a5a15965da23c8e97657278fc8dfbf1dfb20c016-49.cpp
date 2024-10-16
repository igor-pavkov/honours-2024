SPL_METHOD(RecursiveDirectoryIterator, getSubPathname)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(getThis());
	char slash = SPL_HAS_FLAG(intern->flags, SPL_FILE_DIR_UNIXPATHS) ? '/' : DEFAULT_SLASH;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (intern->u.dir.sub_path) {
		RETURN_NEW_STR(strpprintf(0, "%s%c%s", intern->u.dir.sub_path, slash, intern->u.dir.entry.d_name));
	} else {
		RETURN_STRING(intern->u.dir.entry.d_name);
	}
}