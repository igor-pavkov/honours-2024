SPL_METHOD(SplFileInfo, getPathInfo)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(getThis());
	zend_class_entry *ce = intern->info_class;
	zend_error_handling error_handling;

	zend_replace_error_handling(EH_THROW, spl_ce_UnexpectedValueException, &error_handling);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|C", &ce) == SUCCESS) {
		size_t path_len;
		char *path = spl_filesystem_object_get_pathname(intern, &path_len);
		if (path) {
			char *dpath = estrndup(path, path_len);
			path_len = php_dirname(dpath, path_len);
			spl_filesystem_object_create_info(intern, dpath, path_len, 1, ce, return_value);
			efree(dpath);
		}
	}

	zend_restore_error_handling(&error_handling);
}